// "frequency" or "freq" refers to a Hz value, and "pitch" refers to
// a numeric musical note with 0 representing C0, 12 for C1, etc..
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "voice.h"
#include "bitArray.h"
#include "misc.h"
#include "sdlec.h"


#define atomic_t SDL_atomic_t
#define atomic_max INT32_MAX // not sure if this is right...

const double ChromaticRatio = 1.059463094359295264562; // the 12th root of 2
const double A4freq  = 440.0;
const double A4pitch =  57.0;

double freqFromPitch(double pitch) {
	return pow(ChromaticRatio, pitch-A4pitch)*A4freq;
}
void buildSineWave(float *data, int sampleCount) {
	fr (i, sampleCount) data[i] = sin(i*(tau/sampleCount));
}

uint32_t sampleRate = 48000;
uint32_t frameRate = 60;
uint32_t floatStreamSize = 1024; // must be a power of 2
atomic_t globalVolume = {0};
const float practicallySilent = 0.001;

int         voiceCount = 0;
voice      *voices = NULL;
float      *voicesPan = NULL;    // -1.0 is all left, 1.0 is all right
bitBlock_t *voicesEnable = NULL; // voice will be ignored it's bit is false
SDL_mutex **voiceMutexes = NULL;

void setGlobalVolume(float v) {
	if      (v >= 1) SDL_AtomicSet(&globalVolume, atomic_max);
	else if (v <= 0) SDL_AtomicSet(&globalVolume, 0);
	else             SDL_AtomicSet(&globalVolume, v*atomic_max);
}

void loopOsc(osc *o) {
	const double p = o->pos;
	if      (p > 1) o->pos -= (long)p;
	else if (p < 0) o->pos -= (long)p-1;
}
void clampOsc(osc *o) {
	const double p = o->pos;
	if (p > 1) {
		o->pos = 1;
		o->spd = 0;
	}
	else if (p < 0) {
		o->pos = 0;
		o->spd = 0;
	}
}

float readOsc(const osc o) {
	return o.shape[(long)(o.pos * (o.sampleCount-1))] * o.amp;
}

void audioCallback(void *_unused, uint8_t *byteStream, int byteStreamLength) {
	float *floatStream = (float*)byteStream;
	const int enabledVoiceCount = btArCountSet(voicesEnable, voiceCount);
	const float globalVolumeF = (float)SDL_AtomicGet(&globalVolume)/atomic_max;
	if (globalVolumeF <= practicallySilent || enabledVoiceCount < 1) {
		fr (s, floatStreamSize) floatStream[s] = 0; // silence
		return;
	}
	fr (v, voiceCount) {
		if (!btArRead(voicesEnable, v)) continue;
		const double spdModIncrem = voices[v][vo_spdMod].spd / sampleRate;
		const double ampModIncrem = voices[v][vo_ampMod].spd / sampleRate;
		const double rightFactor = (voicesPan[v]+1.0)/2.0;
		const double leftFactor  = 1.0 - rightFactor;
		for (int s = 0; s < floatStreamSize; s += 2) {
			voices[v][vo_spdEnv].pos += voices[v][vo_spdEnv].spd / sampleRate;
			clampOsc(&voices[v][vo_spdEnv]);
			voices[v][vo_spdMod].pos += spdModIncrem;
			loopOsc(&voices[v][vo_spdMod]);
			voices[v][vo_wave].pos += (voices[v][vo_wave].spd * readOsc(voices[v][vo_spdEnv]) * readOsc(voices[v][vo_spdMod])) / sampleRate;
			loopOsc(&voices[v][vo_wave]);
			voices[v][vo_ampEnv].pos += voices[v][vo_ampEnv].spd / sampleRate;
			clampOsc(&voices[v][vo_ampEnv]);
			voices[v][vo_ampMod].pos += ampModIncrem;
			loopOsc(&voices[v][vo_ampMod]);
			const double sample = readOsc(voices[v][vo_wave]) * readOsc(voices[v][vo_ampMod]) * readOsc(voices[v][vo_ampEnv]);
			floatStream[s  ] += sample * leftFactor;
			floatStream[s+1] += sample * rightFactor;
		}
	}
	if (enabledVoiceCount > 1) {
		const float amp = globalVolumeF * (1.0/enabledVoiceCount);
		fr (s, floatStreamSize) floatStream[s] *= amp;
		return;
	}
	fr (s, floatStreamSize) floatStream[s] *= globalVolumeF;
}

SDL_AudioDeviceID AudioDevice;
SDL_AudioSpec audioSpec;

int initVoices(int initVoiceCount) {
	SDL_Init(SDL_INIT_AUDIO);_sdlec;
	SDL_AudioSpec want = {0};
	want.freq     = sampleRate;
	want.format   = AUDIO_F32;
	want.channels = 2; // stereo
	want.samples  = 1024; // must be a power of 2
	want.callback = audioCallback;
	AudioDevice = SDL_OpenAudioDevice(NULL, 0, &want, &audioSpec, 0);_sdlec;
	sampleRate = audioSpec.freq;
	floatStreamSize = audioSpec.size/sizeof(float);
	voiceCount = initVoiceCount;
	voices = calloc(voiceCount, sizeof(voice));
	voicesPan = calloc(voiceCount, sizeof(float));
	voiceMutexes = calloc(voiceCount, sizeof());
	voicesEnable = btArAlloc(voiceCount);
	fr (v, voiceCount) {
		voiceMutexes[v] = SDL_CreateMutex(voiceMutexes[v]);_sdlec;
	}
	return 0;
}
int closeVoices(void) {
	SDL_CloseAudioDevice(AudioDevice);_sdlec;
	//fr (v, voiceCount) {
	//	fr (o, vo_oscPerVoice) {
	//		free(voices[v][o].shape);
	//	}
	//}
	free(voices);
	free(voicesPan);
	free(voicesEnable);
	fr (v, voiceCount) {
		SDL_DestroyMutex(voiceMutexes[v]);_sdlec;
	}
	free(voiceMutexes);
	return 0;
}
void unpauseAudio(void) {SDL_PauseAudioDevice(AudioDevice, 0);_sdlec;}
void   pauseAudio(void) {SDL_PauseAudioDevice(AudioDevice, 1);_sdlec;}

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

void buildSineWave(float *data, int length) {
	fr (i, length) data[i] = sin(i*(tau/length));
}

uint32_t sampleRate = 48000;
uint32_t frameRate = 60;
uint32_t floatStreamSize = 1024; // must be a power of 2
atomic_t globalVolume = {0};

int         voiceCount = 0;
voice      *voices = NULL;
float      *voicesPan = NULL;    // -1.0 is all left, 1.0 is all right
bitBlock_t *voicesEnable = NULL; // voice will be ignored it's bit is false

void setGlobalVolume(float v) {
	if (v >= 1) {
		SDL_AtomicSet(&globalVolume, atomic_max);
		return;
	}
	if (v <= 0) {
		SDL_AtomicSet(&globalVolume, 0);
		return;
	}
	SDL_AtomicSet(&globalVolume, v*atomic_max);
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
	return o.shape[(long)(o.pos * (o.shapeSize-1))] * o.amp;
}

void audioCallback(void *_unused, uint8_t *byteStream, int byteStreamLength) {
	float *floatStream = (float*)byteStream;
	float enabledVoiceCount = 0;
	fr (v, voiceCount) {
		if (!btArRead(voicesEnable, v)) continue;
		enabledVoiceCount++;
		const double spdModIncrem = voices[v].spdMod.spd / sampleRate;
		const double ampModIncrem = voices[v].ampMod.spd / sampleRate;
		const double rightFactor = (voicesPan[v]+1.0)/2.0;
		const double leftFactor  = 1.0 - rightFactor;
		for (int s = 0; s < floatStreamSize; s += 2) {
			voices[v].spdEnv.pos += voices[v].spdEnv.spd / sampleRate;
			clampOsc(&voices[v].spdEnv);
			voices[v].spdMod.pos += spdModIncrem;
			loopOsc(&voices[v].spdMod);
			voices[v].wave.pos += (voices[v].wave.spd * readOsc(voices[v].spdEnv) * readOsc(voices[v].spdMod)) / sampleRate;
			loopOsc(&voices[v].wave);
			voices[v].ampEnv.pos += voices[v].ampEnv.spd / sampleRate;
			clampOsc(&voices[v].ampEnv);
			voices[v].ampMod.pos += ampModIncrem;
			loopOsc(&voices[v].ampMod);
			const double sample = readOsc(voices[v].wave) * readOsc(voices[v].ampMod) * readOsc(voices[v].ampEnv);
			floatStream[s  ] += sample * leftFactor;
			floatStream[s+1] += sample * rightFactor;
		}
	}
	if (enabledVoiceCount > 1) {
		const float amp = ((float)SDL_AtomicGet(&globalVolume)/atomic_max) * (1.0/enabledVoiceCount);
		fr (s, floatStreamSize) floatStream[s] *= amp;
	}
	else if (enabledVoiceCount == 1) {
		const float amp = ((float)SDL_AtomicGet(&globalVolume)/atomic_max);
		fr (s, floatStreamSize) floatStream[s] *= amp;
	}
	else {
		fr (s, floatStreamSize) floatStream[s] = 0; // silence
	}
}

SDL_AudioDeviceID AudioDevice;
SDL_AudioSpec audioSpec;

int initVoices(int initVoiceCount) {
	SDL_Init(SDL_INIT_AUDIO);_sdlec
	SDL_AudioSpec want = {0};
	want.freq     = sampleRate;
	want.format   = AUDIO_F32;
	want.channels = 2; // stereo
	want.samples  = 1024; // must be a power of 2
	want.callback = audioCallback;
	AudioDevice = SDL_OpenAudioDevice(NULL, 0, &want, &audioSpec, 0);_sdlec
	sampleRate = audioSpec.freq;
	floatStreamSize = audioSpec.size/sizeof(float);
	voiceCount = initVoiceCount;
	voices = calloc(voiceCount, sizeof(voice));
	return 0;
}
int closeVoices(void) {
	SDL_CloseAudioDevice(AudioDevice);_sdlec
	free(voices);
	return 0;
}
void unpauseAudio(void) {SDL_PauseAudioDevice(AudioDevice, 0);_sdlec}
void   pauseAudio(void) {SDL_PauseAudioDevice(AudioDevice, 1);_sdlec}

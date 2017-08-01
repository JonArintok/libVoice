// "frequency" or "freq" refers to a Hz value, and "pitch" refers to
// a numeric musical note with 0 representing C0, 12 for C1, etc..
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "voice.h"
#include "misc.h"
#include "sdlec.h"


#define atomic_t SDL_atomic_t
#define atomic_max INT32_MAX // not sure if this is right...

const double chromaticRatio = 1.059463094359295264562; // the 12th root of 2
const double A4freq  = 440.0;
const double A4pitch =  57.0;

double freqFromPitch(double pitch) {
	return pow(chromaticRatio, pitch-A4pitch)*A4freq;
}

uint32_t sampleRate = 48000;
uint32_t frameRate = 60;
uint32_t floatStreamSize = 1024; // must be a power of 2
atomic_t globalVolume = {0};

void setGlobalVolume(float v) {
	if      (v >= 1) {SDL_AtomicSet(&globalVolume, atomic_max);_sdlec;}
	else if (v <= 0) {SDL_AtomicSet(&globalVolume, 0);_sdlec;}
	else             {SDL_AtomicSet(&globalVolume, v*atomic_max);_sdlec;}
}

typedef struct {float *data; long count;} floatArray;
int         shapeCount;
floatArray *shapes = NULL;
floatArray *shapesIn = NULL;
SDL_mutex **shapeMutexes = NULL;
atomic_t    shapesHaveChanged = {0};

void shapeFromMem(int shapeIndex, int sampleCount, float *mem) {
	SDL_LockMutex(shapeMutexes[shapeIndex]);
	shapesIn[shapeIndex].data = realloc(shapesIn[shapeIndex].data, sizeof(float)*sampleCount);
	shapesIn[shapeIndex].count = -sampleCount; // negative count indicates change
	fr (s, sampleCount) shapesIn[shapeIndex].data[s] = mem[s];
	SDL_UnlockMutex(shapeMutexes[shapeIndex]);
}
void shapeFromSine(int shapeIndex, int sampleCount, double low, double high) {
	const double scale = 2.0/(high-low);
	const double shift = low + (high-low)/2.0;
	SDL_LockMutex(shapeMutexes[shapeIndex]);
	shapesIn[shapeIndex].data = realloc(shapesIn[shapeIndex].data, sizeof(float)*sampleCount);
	shapesIn[shapeIndex].count = -sampleCount;
	fr (s, sampleCount) {
		shapesIn[shapeIndex].data[s] = sin(s*(tau/sampleCount))*scale + shift;
	}
	SDL_UnlockMutex(shapeMutexes[shapeIndex]);
}
void shapeFromSaw(int shapeIndex, int sampleCount, double low, double high) {
	const double scale = 2.0/(high-low);
	const double shift = low + (high-low)/2.0;
	SDL_LockMutex(shapeMutexes[shapeIndex]);
	shapesIn[shapeIndex].data = realloc(shapesIn[shapeIndex].data, sizeof(float)*sampleCount);
	shapesIn[shapeIndex].count = -sampleCount;
	fr (s, sampleCount) {
		shapesIn[shapeIndex].data[s] = (1.0 - ((double)s/sampleCount)*2)*scale + shift;
	}
	SDL_UnlockMutex(shapeMutexes[shapeIndex]);
}
void shapeFromPulse(int shapeIndex, int sampleCount, double low, double high, double pulseWidth) {
	const double pw = pulseWidth > 1 ? 1.0 : (pulseWidth < 0 ? 0.0 : pulseWidth);
	SDL_LockMutex(shapeMutexes[shapeIndex]);
	shapesIn[shapeIndex].data = realloc(shapesIn[shapeIndex].data, sizeof(float)*sampleCount);
	shapesIn[shapeIndex].count = -sampleCount;
	int s = 0;
	for (; s < sampleCount*pw; s++) shapesIn[shapeIndex].data[s] = high;
	for (; s < sampleCount;    s++) shapesIn[shapeIndex].data[s] = low;
	SDL_UnlockMutex(shapeMutexes[shapeIndex]);
}

void syncShapes(void) {
	fr (s, shapeCount) {
		if (!SDL_TryLockMutex(shapeMutexes[s])) {
			if (shapesIn[s].count < 0) { // negative count indicates change
				shapesIn[s].count *= -1;
				const long newCount = shapesIn[s].count;
				if (shapes[s].count < newCount) {
					shapes[s].data = realloc(shapes[s].data, sizeof(float)*newCount);
				}
				fr (f, newCount) shapes[s].data[f] = shapesIn[s].data[f];
				shapes[s].count = newCount;
			}
			SDL_UnlockMutex(shapeMutexes[s]);
		}
	}
}


int         voiceCount = 0;
voice      *voices = NULL;
float      *voicesPan = NULL;    // -1.0 is all left, 1.0 is all right
SDL_mutex  *voicesEnableMutex = NULL;
SDL_mutex **voiceMutexes = NULL;

void setOscShape(int voiceIndex, int voicePart, int shapeIndex) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	voices[voiceIndex][voicePart].shape = shapeIndex;
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void setOscAmp(int voiceIndex, int voicePart, double amp) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	voices[voiceIndex][voicePart].amp = amp;
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void setOscPos(int voiceIndex, int voicePart, double pos) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	voices[voiceIndex][voicePart].pos = pos;
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void setOscInc(int voiceIndex, int voicePart, double inc) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	voices[voiceIndex][voicePart].inc = inc;
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void setOscIncFromLoopFreq(int voiceIndex, int voicePart, double freq) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	SDL_LockMutex(shapeMutexes[voices[voiceIndex][voicePart].shape]);
	double shapeLength = shapes[voices[voiceIndex][voicePart].shape].count;
	SDL_UnlockMutex(shapeMutexes[voices[voiceIndex][voicePart].shape]);
	voices[voiceIndex][voicePart].inc = (freq/(sampleRate/shapeLength))/shapeLength;
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void setOscIncFromPlaySpeed(int voiceIndex, int voicePart, double speed) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	SDL_LockMutex(shapeMutexes[voices[voiceIndex][voicePart].shape]);
	double shapeLength = shapes[voices[voiceIndex][voicePart].shape].count;
	SDL_UnlockMutex(shapeMutexes[voices[voiceIndex][voicePart].shape]);
	voices[voiceIndex][voicePart].inc = speed/shapeLength;
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void setOsc(int voiceIndex, int voicePart, osc o) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	voices[voiceIndex][voicePart] = o;
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void setVoice(int voiceIndex, voice v) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	fr (o, vo_oscPerVoice) voices[voiceIndex][o] = v[o];
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void enableVoice(int voiceIndex) {
	SDL_LockMutex(voicesEnableMutex);_sdlec;
	voices[voiceIndex][vo_wave].shape = abs(voices[voiceIndex][vo_wave].shape);
	SDL_UnlockMutex(voicesEnableMutex);_sdlec;
}
void disableVoice(int voiceIndex) {
	SDL_LockMutex(voicesEnableMutex);_sdlec;
	voices[voiceIndex][vo_wave].shape = -1*abs(voices[voiceIndex][vo_wave].shape);
	SDL_UnlockMutex(voicesEnableMutex);_sdlec;
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
		o->inc = 0;
	}
	else if (p < 0) {
		o->pos = 0;
		o->inc = 0;
	}
}

float readOsc(const osc o) {
	return shapes[o.shape].data[(long)(o.pos * (shapes[o.shape].count-1))] * o.amp;
}


// TEMP
//#define audioHistoryLength 2048
//float   audioHistory[audioHistoryLength];
//int     audioHistoryPos = 0;



void audioCallback(void *_unused, uint8_t *byteStream, int byteStreamLength) {
	syncShapes();
	float *floatStream = (float*)byteStream;
	int enabledVoiceCount = 0;
	fr (s, floatStreamSize) floatStream[s] = 0;
	fr (v, voiceCount) {
		SDL_LockMutex(voiceMutexes[v]);_sdlec;
		if (voices[v][vo_wave].shape < 0) continue;
		enabledVoiceCount++;
		const double rightFactor = (voicesPan[v]+1.0)/2.0;
		const double leftFactor  = 1.0 - rightFactor;
		for (int s = 0; s < floatStreamSize; s += 2) {
			voices[v][vo_incEnv].pos += voices[v][vo_incEnv].inc;
			clampOsc(&voices[v][vo_incEnv]);
			voices[v][vo_incMod].pos += voices[v][vo_incMod].inc;
			loopOsc(&voices[v][vo_incMod]);
			voices[v][vo_wave].pos += voices[v][vo_wave].inc;// (voices[v][vo_wave].inc * readOsc(voices[v][vo_incEnv]) * readOsc(voices[v][vo_incMod]));
			loopOsc(&voices[v][vo_wave]);
			//if (audioHistoryPos < audioHistoryLength) audioHistory[audioHistoryPos++] = readOsc(voices[v][vo_wave]); // TEMP
			voices[v][vo_ampEnv].pos += voices[v][vo_ampEnv].inc;
			clampOsc(&voices[v][vo_ampEnv]);
			voices[v][vo_ampMod].pos += voices[v][vo_ampMod].inc;
			loopOsc(&voices[v][vo_ampMod]);
			const double sample = readOsc(voices[v][vo_wave]) * readOsc(voices[v][vo_ampMod]) * readOsc(voices[v][vo_ampEnv]);
			floatStream[s  ] += sample * leftFactor;
			floatStream[s+1] += sample * rightFactor;
		}
		SDL_UnlockMutex(voiceMutexes[v]);_sdlec;
	}
	if (enabledVoiceCount < 1) return;
	const float globalVolumeF = (float)SDL_AtomicGet(&globalVolume)/atomic_max;_sdlec;
	if (enabledVoiceCount > 1) {
		const double amp = globalVolumeF / enabledVoiceCount;
		fr (s, floatStreamSize) floatStream[s] *= amp;
	}
	else fr (s, floatStreamSize) floatStream[s] *= globalVolumeF;
}

SDL_AudioDeviceID audioDevice;
SDL_AudioSpec audioSpec;

int initVoices(int initVoiceCount, int initShapeCount) {
	SDL_Init(SDL_INIT_AUDIO);_sdlec;
	SDL_AudioSpec want = {0};
	want.freq     = sampleRate;
	want.format   = AUDIO_F32;
	want.channels = 2; // stereo
	want.samples  = 1024; // must be a power of 2
	want.callback = audioCallback;
	audioDevice = SDL_OpenAudioDevice(NULL, 0, &want, &audioSpec, 0);_sdlec;
	sampleRate = audioSpec.freq;
	floatStreamSize = audioSpec.size/sizeof(float);
	// shape data
	shapeCount = initShapeCount;
	shapes   = malloc(sizeof(floatArray)*shapeCount);
	shapesIn = malloc(sizeof(floatArray)*shapeCount);
	fr (s, shapeCount) {
		shapes  [s].data = malloc(sizeof(float));
		shapesIn[s].data = malloc(sizeof(float));
		shapes  [s].count = 1;
		shapesIn[s].count = 1;
		shapes  [s].data[0] = 1.0;
		shapesIn[s].data[0] = 1.0;
	}
	shapeMutexes = malloc(sizeof(SDL_mutex*)*shapeCount);
	fr (s, shapeCount) {shapeMutexes[s] = SDL_CreateMutex();_sdlec;}
	// voice data
	voiceCount = initVoiceCount;
	voices = calloc(voiceCount, sizeof(voice));
	voicesPan = calloc(voiceCount, sizeof(float));
	voiceMutexes = malloc(sizeof(SDL_mutex*)*voiceCount);
	fr (v, voiceCount) {voiceMutexes[v] = SDL_CreateMutex();_sdlec;}
	setGlobalVolume(1.0);
	return 0;
}

int closeVoices(void) {
	SDL_CloseAudioDevice(audioDevice);_sdlec;
	
	// TEMP
	//puts("\n________audioHistory________");
	//fr (s, audioHistoryLength) printf("%4i: %7.6f\n", s, audioHistory[s]);
	//puts("");
	
	fr (s, shapeCount) {
		SDL_LockMutex(shapeMutexes[s]);
		free(shapes[s].data);
		free(shapesIn[s].data);
		SDL_UnlockMutex(shapeMutexes[s]);
		SDL_DestroyMutex(shapeMutexes[s]);_sdlec;
	}
	free(shapes);
	free(shapesIn);
	free(shapeMutexes);
	free(voices);
	free(voicesPan);
	fr (v, voiceCount) {SDL_DestroyMutex(voiceMutexes[v]);_sdlec;}
	free(voiceMutexes);
	return 0;
}

void unpauseAudio(void) {SDL_PauseAudioDevice(audioDevice, 0);_sdlec;}
void   pauseAudio(void) {SDL_PauseAudioDevice(audioDevice, 1);_sdlec;}

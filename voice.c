// "frequency" or "freq" refers to a Hz value, and "pitch" refers to
// a numeric musical note with 0 for C0, 12 for C1, etc..
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "voice.h"
#include "misc.h"
#include "sdlec.h"

//#define LOG_AUDIO_HISTORY


#define atomic_t SDL_atomic_t
#define atomic_max INT32_MAX // not sure if this is right...

const double semitoneRatio = 1.059463094359295264562; // the 12th root of 2
const double A4freq  = 440.0;
const double A4pitch =  57.0;

double freqFromPitch(double pitch) {
	return pow(semitoneRatio, pitch-A4pitch)*A4freq;
}

uint32_t sampleRate = 48000; // may be changed by initVoices(), but not after
uint32_t floatStreamSize = 1024; // must be a power of 2
atomic_t globalVolume = {0}; // atomic_max is 1.0


void setGlobalVolume(float v) {
	if      (v >= 1) {SDL_AtomicSet(&globalVolume, atomic_max);_sdlec;}
	else if (v <= 0) {SDL_AtomicSet(&globalVolume, 0);_sdlec;}
	else             {SDL_AtomicSet(&globalVolume, v*atomic_max);_sdlec;}
}
float getGlobalVolume(void) {
	return pow((float)SDL_AtomicGet(&globalVolume)/atomic_max, 2);_sdlec;
}

typedef struct {float *data; long count;} floatArray;
int         shapeCount;
floatArray *shapes = NULL;
floatArray *shapesIn = NULL;
SDL_mutex **shapeMutexes = NULL;
atomic_t    shapesHaveChanged = {0};

void shapeFromMem(int shapeIndex, int sampleCount, const float *mem) {
	SDL_LockMutex(shapeMutexes[shapeIndex]);
	shapesIn[shapeIndex].data = realloc(shapesIn[shapeIndex].data, sizeof(float)*sampleCount);
	shapesIn[shapeIndex].count = -sampleCount; // negative count indicates change
	fr (s, sampleCount) {
		shapesIn[shapeIndex].data[s] = mem[s];
		//printf("shapesIn[%i].data[%i]: %f\n", shapeIndex, s, shapesIn[shapeIndex].data[s]);
	}
	SDL_UnlockMutex(shapeMutexes[shapeIndex]);
}
void shapeFromSine(int shapeIndex, int sampleCount) {
	SDL_LockMutex(shapeMutexes[shapeIndex]);
	shapesIn[shapeIndex].data = realloc(shapesIn[shapeIndex].data, sizeof(float)*sampleCount);
	shapesIn[shapeIndex].count = -sampleCount;
	fr (s, sampleCount) {
		shapesIn[shapeIndex].data[s] = sin(s*(tau/sampleCount));
	}
	SDL_UnlockMutex(shapeMutexes[shapeIndex]);
}
void shapeFromSaw(int shapeIndex, int sampleCount) {
	SDL_LockMutex(shapeMutexes[shapeIndex]);
	shapesIn[shapeIndex].data = realloc(shapesIn[shapeIndex].data, sizeof(float)*sampleCount);
	shapesIn[shapeIndex].count = -sampleCount;
	fr (s, sampleCount) {
		shapesIn[shapeIndex].data[s] = (1.0 - ((double)s/sampleCount)*2);
	}
	SDL_UnlockMutex(shapeMutexes[shapeIndex]);
}
void shapeFromPulse(int shapeIndex, int sampleCount, double pulseWidth) {
	const double pw = pulseWidth > 1 ? 1.0 : (pulseWidth < 0 ? 0.0 : pulseWidth);
	SDL_LockMutex(shapeMutexes[shapeIndex]);
	shapesIn[shapeIndex].data = realloc(shapesIn[shapeIndex].data, sizeof(float)*sampleCount);
	shapesIn[shapeIndex].count = -sampleCount;
	int s = 0;
	for (; s < sampleCount*pw; s++) shapesIn[shapeIndex].data[s] =  1.0;
	for (; s < sampleCount;    s++) shapesIn[shapeIndex].data[s] = -1.0;
	SDL_UnlockMutex(shapeMutexes[shapeIndex]);
}

SDL_AudioSpec audioSpec;
void logSpec(const SDL_AudioSpec as) {
  printf(
    " freq______%5d\n"
    " format____%5d\n"
    " channels__%5d\n"
    " silence___%5d\n"
    " samples___%5d\n"
    " size______%5d\n\n",
    as.freq,
    as.format,
    as.channels,
    as.silence,
    as.samples,
    as.size
  );
}
void shapesFromWavFile(int firstShapeIndex, uint32_t shapeCount, const char *path) {
	float *samples;
	int sampleCount;
	{
		SDL_AudioSpec wavSpec;
		uint8_t      *wavBuf;
		uint32_t      wavBufSize;
		if (!SDL_LoadWAV(path, &wavSpec, &wavBuf, &wavBufSize)) {
			printf("\ncould not load file: \"%s\"\n", path);
			return;
		}
		logSpec(wavSpec);
		SDL_AudioCVT cvt;
		// if channelSelect < 0 or past the available channels,
		// then let SDL mix it, else we will extract the desired channel later
		if (shapeCount > wavSpec.channels) shapeCount = wavSpec.channels;
		SDL_BuildAudioCVT(
			&cvt,             // SDL_AudioCVT*   cvt
			wavSpec.format,   // SDL_AudioFormat src_format
			wavSpec.channels, // Uint8           src_channels
			wavSpec.freq,     // int             src_rate
			audioSpec.format, // SDL_AudioFormat dst_format
			shapeCount,       // Uint8           dst_channels
			audioSpec.freq    // int             dst_rate
		);_sdlec;
		SDL_assert(cvt.needed);
		cvt.len = wavBufSize;
		cvt.len_cvt = wavBufSize*cvt.len_mult;
		cvt.buf = malloc(cvt.len_cvt*sizeof(uint8_t));
		fr (i, wavBufSize) cvt.buf[i] = wavBuf[i];
		SDL_ConvertAudio(&cvt);_sdlec;
		samples = (float*)cvt.buf;
		sampleCount = cvt.len_cvt/sizeof(float);
		SDL_FreeWAV(wavBuf);_sdlec;
	}
	// printf("sampleCount: %i\n", sampleCount);
	// puts("\n_______samples_______");
	// fr (s, sampleCount) {
	// 	printf("%4i: %7.6f\n", s, samples[s]);
	// }
	// puts("");
	if (shapeCount == 1) {
		shapeFromMem(firstShapeIndex, sampleCount, samples);
	}
	else {
		
		fr (c, shapeCount) {
			
		}
	}
	free(samples);
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
float      *voicesPan = NULL;
SDL_mutex **voiceMutexes = NULL;

void setOscShape(int voiceIndex, int voicePart, int shapeIndex) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	voices[voiceIndex][voicePart].shape = shapeIndex;
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void setOscShift(int voiceIndex, int voicePart, double shift) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	voices[voiceIndex][voicePart].shift = shift;
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
void setOscIncFromFreq(int voiceIndex, int voicePart, double freq) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	SDL_LockMutex(shapeMutexes[voices[voiceIndex][voicePart].shape]);
	double shapeLength = abs(shapesIn[voices[voiceIndex][voicePart].shape].count);
	SDL_UnlockMutex(shapeMutexes[voices[voiceIndex][voicePart].shape]);
	voices[voiceIndex][voicePart].inc = (freq/(sampleRate/shapeLength))/shapeLength;
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void setOscIncFromPeriod(int voiceIndex, int voicePart, double period) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	voices[voiceIndex][voicePart].inc = (1.0/sampleRate)/period;
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void setOscIncFromSpeed(int voiceIndex, int voicePart, double speed) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	SDL_LockMutex(shapeMutexes[voices[voiceIndex][voicePart].shape]);
	const double shapeLength = abs(shapesIn[voices[voiceIndex][voicePart].shape].count);
	SDL_UnlockMutex(shapeMutexes[voices[voiceIndex][voicePart].shape]);
	voices[voiceIndex][voicePart].inc = speed/shapeLength;
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void setOsc(int voiceIndex, int voicePart, const osc o) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	voices[voiceIndex][voicePart] = o;
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void setVoice(int voiceIndex, const voice v) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	fr (o, vo_oscPerVoice) voices[voiceIndex][o] = v[o];
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void restartVoice(int voiceIndex) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	fr (o, vo_oscPerVoice) voices[voiceIndex][o].pos = 0;
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void enableVoice(int voiceIndex) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	voices[voiceIndex][vo_wave].shape = abs(voices[voiceIndex][vo_wave].shape);
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void disableVoice(int voiceIndex) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	voices[voiceIndex][vo_wave].shape = -1*abs(voices[voiceIndex][vo_wave].shape);
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}
void setVoicePan(int voiceIndex, double pan) {
	SDL_LockMutex(voiceMutexes[voiceIndex]);
	voicesPan[voiceIndex] = pan;
	SDL_UnlockMutex(voiceMutexes[voiceIndex]);
}


// the following functions expect the containing voice to be locked already
void loopOsc(osc *o) {
	const double p = o->pos;
	if      (p > 1) o->pos -= (long)p;
	else if (p < 0) o->pos -= (long)p-1;
}
void clampOsc(osc *o) {
	const double p = o->pos;
	if      (p > 1) o->pos = 1;
	else if (p < 0) o->pos = 0;
}

float readOsc(const osc o) {
	return shapes[o.shape].data[(long)(o.pos * (shapes[o.shape].count-1))] * o.amp + o.shift;
}


#ifdef LOG_AUDIO_HISTORY
#define audioHistoryLength 2048
float   audioHistory[audioHistoryLength];
int     audioHistoryPos = 0;
#endif

void breakPoint(void) {return;}

void audioCallback(void *_unused, uint8_t *byteStream, int byteStreamLength) {
	syncShapes();
	float *floatStream = (float*)byteStream;
	int enabledVoiceCount = 0;
	fr (s, floatStreamSize) floatStream[s] = 0;
	fr (v, voiceCount) {
		SDL_LockMutex(voiceMutexes[v]);_sdlec;
		if (voices[v][vo_wave].shape < 0) {
			SDL_UnlockMutex(voiceMutexes[v]);_sdlec;
			continue;
		}
		enabledVoiceCount++;
		const double rightFactor = fabs(sin(((voicesPan[v]+1.0)*M_PI)/4));
		const double leftFactor  = fabs(sin(((voicesPan[v]-1.0)*M_PI)/4));
		for (int s = 0; s < floatStreamSize; s += 2) {
			voices[v][vo_incEnv].pos += voices[v][vo_incEnv].inc;
			clampOsc(&voices[v][vo_incEnv]);
			voices[v][vo_incMod].pos += voices[v][vo_incMod].inc;
			loopOsc(&voices[v][vo_incMod]);
			voices[v][vo_wave].pos += (voices[v][vo_wave].inc * readOsc(voices[v][vo_incEnv]) * readOsc(voices[v][vo_incMod]));
			loopOsc(&voices[v][vo_wave]);
			voices[v][vo_ampEnv].pos += voices[v][vo_ampEnv].inc;
			clampOsc(&voices[v][vo_ampEnv]);
			voices[v][vo_ampMod].pos += voices[v][vo_ampMod].inc;
			loopOsc(&voices[v][vo_ampMod]);
			const double sample = readOsc(voices[v][vo_wave]) * readOsc(voices[v][vo_ampMod]) * readOsc(voices[v][vo_ampEnv]);
			#ifdef LOG_AUDIO_HISTORY
			if (audioHistoryPos < audioHistoryLength) audioHistory[audioHistoryPos++] = sample; // TEMP
			if (audioHistoryPos > 511) {
				breakPoint();
			}
			#endif
			floatStream[s  ] += sample * leftFactor;
			floatStream[s+1] += sample * rightFactor;
		}
		SDL_UnlockMutex(voiceMutexes[v]);_sdlec;
	}
	if (enabledVoiceCount < 1) return;
	const float globalVolumeF = getGlobalVolume();
	if (enabledVoiceCount > 1) {
		const double amp = globalVolumeF / enabledVoiceCount;
		fr (s, floatStreamSize) floatStream[s] *= amp;
	}
	else fr (s, floatStreamSize) floatStream[s] *= globalVolumeF;
}

SDL_AudioDeviceID audioDevice;

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
	
	#ifdef LOG_AUDIO_HISTORY
	puts("\n________audioHistory________");
	fr (s, audioHistoryLength) {
		printf("%4i: %7.6f", s, audioHistory[s]);
		if (audioHistory[s] > 1 || audioHistory[s] < -1) puts("CLIPPING!");
		else puts("");
	}
	puts("");
	#endif
	
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

void unpauseAudio(void) {
	syncShapes();
	SDL_PauseAudioDevice(audioDevice, 0);_sdlec;
}
void pauseAudio(void) {SDL_PauseAudioDevice(audioDevice, 1);_sdlec;}

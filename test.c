

#include <SDL2/SDL.h>
#include <math.h>
#include "voice.h"
#include "sdlec.h"

enum {
	shape_oneOne,
	shape_sineWav,
	shape_sawWav,
	shape_pulseWav,
	shape_fromFile_00,
	shape_COUNT
};
enum {
	voice_00,
	voice_01,
	voice_02,
	voice_03,
	voice_04,
	voice_05,
	voice_06,
	voice_07,
	voice_COUNT
};

int main(int argc, char **argv) {
	SDL_Init(SDL_INIT_TIMER);_sdlec;
	initVoices(voice_COUNT, shape_COUNT);
	{
		float oneOne[1] = {1};
		shapeFromMem(shape_oneOne, 1, oneOne);
	}
	shapeFromSine (shape_sineWav, 1024);
	shapeFromSaw  (shape_sawWav,  1024);
	shapeFromPulse(shape_pulseWav, 4, 0.5); // doesn't work with sampleCount 2...
	
	{
		voice v = {
			// shape,         shift, amp,  pos, inc
			{  shape_sawWav,  0.0,   1.0,  0.0, 0.0 }, // wave
			{  shape_sawWav,  0.75,  0.0,  0.0, 1.0 }, // ampMod
			{  shape_sineWav, 0.80,  0.0,  0.0, 1.0 }, // spdMod
			{  shape_oneOne,  0.5,   0.5,  0.0, 0.0 }, // ampEnv
			{  shape_oneOne,  0.5,   0.5,  0.0, 0.0 }  // spdEnv
		};
		setVoice(voice_00, v);
	}
	setOscIncFromFreq(voice_00, vo_wave, 440.0);
	setOscIncFromFreq(voice_00, vo_ampMod, 5.0); // pulse 5 times per second
	setOscIncFromFreq(voice_00, vo_incMod, 8.0); // pulse 2 times per second
	setGlobalVolume(0);
	
	/*
	puts("unpauseAudio()");
	unpauseAudio();
	for (double v = 0.0; v <= 0.9; v += 0.02) {
		printf("setGlobalVolume(%f)\n", v);
		setGlobalVolume(v);
		SDL_Delay(16);
	}
	SDL_Delay(1000); puts("");
	{
		double p = 0;
		for (; p > -1.0; p -= 0.1) {
			printf("setVoicePan(voice_00, %f)\n", p);
			setVoicePan(voice_00, p);
			SDL_Delay(16);
		}
		SDL_Delay(1000);
		for (; p < 1.0; p += 0.1) {
			printf("setVoicePan(voice_00, %f)\n", p);
			setVoicePan(voice_00, p);
			SDL_Delay(16);
		}
		SDL_Delay(1000);
		for (; p >= 0.0; p -= 0.1) {
			printf("setVoicePan(voice_00, %f)\n", p);
			setVoicePan(voice_00, p);
			SDL_Delay(16);
		}
	}
	SDL_Delay(1000); puts("");
	
	{
		double ama = 0.0;
		for (; ama < 0.24; ama += 0.005) {
			printf("setOscAmp(voice_00, vo_ampMod, %f)\n", ama);
			setOscAmp(voice_00, vo_ampMod, ama);
			SDL_Delay(16);
		}
		SDL_Delay(1000); puts("");
		double sma = 0.0;
		for (; sma < 0.02; sma += 0.001) {
			printf("setOscAmp(voice_00, vo_incMod, %f)\n", sma);
			setOscAmp(voice_00, vo_incMod, sma);
			SDL_Delay(16);
		}
		SDL_Delay(1000); puts("");
		for (; ama >= 0.0; ama -= 0.005) {
			printf("setOscAmp(voice_00, vo_ampMod, %f)\n", ama);
			setOscAmp(voice_00, vo_ampMod, ama);
			SDL_Delay(16);
		}
		SDL_Delay(1000); puts("");
		for (; sma >= 0; sma -= 0.001) {
			printf("setOscAmp(voice_00, vo_incMod, %f)\n", sma);
			setOscAmp(voice_00, vo_incMod, sma);
			SDL_Delay(16);
		}
		SDL_Delay(1000); puts("");
	}
	
	puts("setOscShape(voice_00, vo_ampEnv, shape_sawWav)");
	puts("setOscIncFromPeriod(voice_00, vo_ampEnv, 0.2)");
	puts("restartVoice(voice_00)");
	setOscShape(voice_00, vo_ampEnv, shape_sawWav);
	setOscIncFromPeriod(voice_00, vo_ampEnv, 0.2); // stretch envelope to last 0.5 seconds
	restartVoice(voice_00);
	SDL_Delay(600);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(600);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(150);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(150);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(150);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(150);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(600); puts("");
	
	puts("setOscShape(voice_00, vo_incEnv, shape_sawWav)");
	puts("setOscIncFromPeriod(voice_00, vo_incEnv, 0.2)");
	puts("restartVoice(voice_00)");
	setOscShape(voice_00, vo_incEnv, shape_sawWav);
	setOscIncFromPeriod(voice_00, vo_incEnv, 0.2);
	restartVoice(voice_00);
	SDL_Delay(600);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(600);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(150);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(150);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(150);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(150);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(600); puts("");
	
	puts("setOscAmp(voice_00, vo_ampMod, 0.24)");
	puts("setOscAmp(voice_00, vo_incMod, 0.02)");
	puts("setOscIncFromPeriod(voice_00, vo_ampEnv, 3.0)");
	puts("setOscIncFromPeriod(voice_00, vo_incEnv, 3.0)");
	puts("restartVoice(voice_00)");
	setOscAmp(voice_00, vo_ampMod, 0.24);
	setOscAmp(voice_00, vo_incMod, 0.02);
	setOscIncFromPeriod(voice_00, vo_ampEnv, 3.0);
	setOscIncFromPeriod(voice_00, vo_incEnv, 3.0);
	restartVoice(voice_00);
	SDL_Delay(4000); puts("");
	
	for (int i = 0; i < voice_COUNT; i++) {
		const voice v = {
			// shape,         shift, amp,  pos, inc
			{  shape_pulseWav, 0.0,   1.0,  0.0, 0.0 }, // wave
			{  shape_oneOne,   0.0,   1.0,  0.0, 0.0 }, // ampMod
			{  shape_oneOne,   0.0,   1.0,  0.0, 0.0 }, // spdMod
			{  shape_oneOne,   0.0,   1.0,  0.0, 0.0 }, // ampEnv
			{  shape_oneOne,   0.0,   1.0,  0.0, 0.0 }  // spdEnv
		};
		setVoice(i, v);
		setVoicePan(i, ((double)i/voice_COUNT)*(-2*(i%2) + 1)); // alternate left/right moving out from center
		const double basePitch = 46; // C4
		#define notesPerChord 4
		const double pitchIntervals[notesPerChord] = {0.0, 4.0, 7.0, 9.0}; // 6 chord
		setOscIncFromFreq(i, vo_wave, freqFromPitch(basePitch + 12*(i/notesPerChord) + pitchIntervals[i%notesPerChord]));
		SDL_Delay(500);
	}
	SDL_Delay(1000); puts("");
	
	{
		double pw = 0.49;
		do {
			pw += 0.01;
			printf("shapeFromPulse(shape_pulseWav, 64, %f);\n", pw);
			shapeFromPulse(shape_pulseWav, 64, pw);
			SDL_Delay(32);
		} while (pw < 1.0);
		do {
			pw -= 0.010;
			printf("shapeFromPulse(shape_pulseWav, 64, %f);\n", pw);
			shapeFromPulse(shape_pulseWav, 64, pw);
			SDL_Delay(32);
		} while (pw > 0.000);
	}
	SDL_Delay(1000); puts("");
	*/
	{
		const voice v = {
			// shape,             shift, amp,  pos, inc
			{  shape_fromFile_00, 0.0,   1.0,  0.0, 0.0 }, // wave
			{  shape_oneOne,      0.0,   1.0,  0.0, 0.0 }, // ampMod
			{  shape_oneOne,      0.0,   1.0,  0.0, 0.0 }, // spdMod
			{  shape_oneOne,      0.0,   1.0,  0.0, 0.0 }, // ampEnv
			{  shape_oneOne,      0.0,   1.0,  0.0, 0.0 }  // spdEnv
		};
		setVoice(0, v);
	}
	shapesFromWavFile(shape_fromFile_00, 1, "GoodEveningRadioAudience.wav");
	setOscIncFromSpeed(0, vo_wave, 1.0);
	setGlobalVolume(0.9);
	
	puts("unpauseAudio()");
	unpauseAudio();
	
	SDL_Delay(5000); puts("");
	
	closeVoices();
	SDL_Quit();_sdlec;
	return 0;
}

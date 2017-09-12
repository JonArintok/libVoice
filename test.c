

#include <SDL2/SDL.h>
#include <math.h>
#include "voice.h"
#include "sdlec.h"

enum {
	shape_oneOne,
	shape_sineWav,
	shape_sawWav,
	shape_squareWav,
	shape_pulseWav,
	shape_goodEvening,
	shape_hellooo,
	shape_ropeSwooshL,
	shape_ropeSwooshR,
	shape_COUNT
};
enum {
	voice_miscCount = 9,
	voice_goodEvening,
	voice_ropeSwooshL,
	voice_ropeSwooshR,
	voice_COUNT
};

void fadeIn(void) {
	voice v = {
		// shape,         amp, shift, pos, inc
		{  shape_sawWav,  1.0, 0.0,   0.0, 0.0 }, // wave
		{  shape_sawWav,  0.0, 0.75,  0.0, 1.0 }, // ampMod
		{  shape_sineWav, 0.0, 0.80,  0.0, 1.0 }, // spdMod
		{  shape_oneOne,  0.5, 0.5,   0.0, 0.0 }, // ampEnv
		{  shape_oneOne,  0.5, 0.5,   0.0, 0.0 }  // spdEnv
	};
	setVoice(0, v);
	setOscIncFromFreq(0, vo_wave, 440.0);
	setOscIncFromFreq(0, vo_ampMod, 5.0); // pulse 5 times per second
	setOscIncFromFreq(0, vo_incMod, 8.0); // pulse 2 times per second
	setGlobalVolume(0);
	for (double v = 0.0; v <= 0.9; v += 0.02) {
		printf("setGlobalVolume(%f)\n", v);
		setGlobalVolume(v);
		SDL_Delay(16);
	}
}

void pan(void) {
	double p = 0;
	for (; p > -1.0; p -= 0.1) {
		printf("setVoicePan(0, %f)\n", p);
		setVoicePan(0, p);
		SDL_Delay(16);
	}
	SDL_Delay(1000);
	for (; p < 1.0; p += 0.1) {
		printf("setVoicePan(0, %f)\n", p);
		setVoicePan(0, p);
		SDL_Delay(16);
	}
	SDL_Delay(1000);
	for (; p >= 0.0; p -= 0.1) {
		printf("setVoicePan(0, %f)\n", p);
		setVoicePan(0, p);
		SDL_Delay(16);
	}
}

void modulate(void) {
	double ama = 0.0;
	for (; ama < 0.24; ama += 0.005) {
		printf("setOscAmp(0, vo_ampMod, %f)\n", ama);
		setOscAmp(0, vo_ampMod, ama);
		SDL_Delay(16);
	}
	SDL_Delay(1000); puts("");
	double sma = 0.0;
	for (; sma < 0.02; sma += 0.001) {
		printf("setOscAmp(0, vo_incMod, %f)\n", sma);
		setOscAmp(0, vo_incMod, sma);
		SDL_Delay(16);
	}
	SDL_Delay(1000); puts("");
	for (; ama >= 0.0; ama -= 0.005) {
		printf("setOscAmp(0, vo_ampMod, %f)\n", ama);
		setOscAmp(0, vo_ampMod, ama);
		SDL_Delay(16);
	}
	SDL_Delay(1000); puts("");
	for (; sma >= 0; sma -= 0.001) {
		printf("setOscAmp(0, vo_incMod, %f)\n", sma);
		setOscAmp(0, vo_incMod, sma);
		SDL_Delay(16);
	}
}

void envelope(void) {
	puts("setOscShape(0, vo_ampEnv, shape_sawWav)");
	puts("setOscIncFromPeriod(0, vo_ampEnv, 0.2)");
	puts("restartVoice(0)");
	setOscShape(0, vo_ampEnv, shape_sawWav);
	setOscIncFromPeriod(0, vo_ampEnv, 0.2); // stretch envelope to last 0.5 seconds
	restartVoice(0);
	SDL_Delay(600);
	puts("restartVoice(0)");
	restartVoice(0);
	SDL_Delay(600);
	puts("restartVoice(0)");
	restartVoice(0);
	SDL_Delay(150);
	puts("restartVoice(0)");
	restartVoice(0);
	SDL_Delay(150);
	puts("restartVoice(0)");
	restartVoice(0);
	SDL_Delay(150);
	puts("restartVoice(0)");
	restartVoice(0);
	SDL_Delay(150);
	puts("restartVoice(0)");
	restartVoice(0);
	SDL_Delay(600); puts("");
	
	puts("setOscShape(0, vo_incEnv, shape_sawWav)");
	puts("setOscIncFromPeriod(0, vo_incEnv, 0.2)");
	puts("restartVoice(0)");
	setOscShape(0, vo_incEnv, shape_sawWav);
	setOscIncFromPeriod(0, vo_incEnv, 0.2);
	restartVoice(0);
	SDL_Delay(600);
	puts("restartVoice(0)");
	restartVoice(0);
	SDL_Delay(600);
	puts("restartVoice(0)");
	restartVoice(0);
	SDL_Delay(150);
	puts("restartVoice(0)");
	restartVoice(0);
	SDL_Delay(150);
	puts("restartVoice(0)");
	restartVoice(0);
	SDL_Delay(150);
	puts("restartVoice(0)");
	restartVoice(0);
	SDL_Delay(150);
	puts("restartVoice(0)");
	restartVoice(0);
	SDL_Delay(600); puts("");
	
	puts("setOscAmp(0, vo_ampMod, 0.24)");
	puts("setOscAmp(0, vo_incMod, 0.02)");
	puts("setOscIncFromPeriod(0, vo_ampEnv, 3.0)");
	puts("setOscIncFromPeriod(0, vo_incEnv, 3.0)");
	puts("restartVoice(0)");
	setOscAmp(0, vo_ampMod, 0.24);
	setOscAmp(0, vo_incMod, 0.02);
	setOscIncFromPeriod(0, vo_ampEnv, 3.0);
	setOscIncFromPeriod(0, vo_incEnv, 3.0);
	restartVoice(0);
}

void manyVoices(void) {
	for (int i = 0; i < voice_miscCount; i++) {
		const voice v = {
			// shape,          amp, shift, pos, inc
			{  shape_pulseWav, 1.0, 0.0,   0.0, 0.0 }, // wave
			{  shape_oneOne,   1.0, 0.0,   0.0, 0.0 }, // ampMod
			{  shape_oneOne,   1.0, 0.0,   0.0, 0.0 }, // spdMod
			{  shape_oneOne,   1.0, 0.0,   0.0, 0.0 }, // ampEnv
			{  shape_oneOne,   1.0, 0.0,   0.0, 0.0 }  // spdEnv
		};
		setVoice(i, v);
		setVoicePan(i, ((double)i/voice_miscCount)*(-2*(i%2) + 1)); // alternate left/right moving out from center
		const double basePitch = 46; // C4
		#define notesPerChord 4
		const double pitchIntervals[notesPerChord] = {0.0, 4.0, 7.0, 9.0}; // 6 chord
		setOscIncFromFreq(i, vo_wave, freqFromPitch(basePitch + 12*(i/notesPerChord) + pitchIntervals[i%notesPerChord]));
		SDL_Delay(500);
	}
}

void pulseWidthSweep(void) {
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

void goodEvening(void) {
	const voice v = {
		// shape,              amp, shift, pos, inc
		{  shape_goodEvening, 16.0, 0.0,   0.0, 0.0 }, // wave
		{  shape_oneOne,       1.0, 0.0,   0.0, 0.0 }, // ampMod
		{  shape_oneOne,       1.0, 0.0,   0.0, 0.0 }, // spdMod
		{  shape_squareWav,    0.5, 0.5,   0.0, 0.0 }, // ampEnv
		{  shape_oneOne,       1.0, 0.0,   0.0, 0.0 }  // spdEnv
	};
	setVoice(voice_goodEvening, v);
	setOscIncFromSpeed(voice_goodEvening, vo_wave, 1.0);
	setOscIncFromPeriod(voice_goodEvening, vo_ampEnv, 4.8); // twice the period, putting it on the high part of the squarewave
	SDL_Delay(3000);
	disableVoice(voice_goodEvening);
	restartVoice(voice_goodEvening);
	setOscPos(voice_goodEvening, vo_wave, 1.0);
	mulOscInc(voice_goodEvening, vo_wave, -1.0);
	enableVoice(voice_goodEvening);
}

void hellooo(void) {
	for (int i = 0; i < voice_miscCount; i++) {
		const voice v = {
			// shape,          amp, shift, pos, inc
			{  shape_hellooo, 16.0, 0.0,   0.0, 0.0 }, // wave
			{  shape_oneOne,   1.0, 0.0,   0.0, 0.0 }, // ampMod
			{  shape_oneOne,   1.0, 0.0,   0.0, 0.0 }, // spdMod
			{  shape_oneOne,   1.0, 0.0,   0.0, 0.0 }, // ampEnv
			{  shape_oneOne,   1.0, 0.0,   0.0, 0.0 }  // spdEnv
		};
		setVoice(i, v);
		setVoicePan(i, ((double)i/voice_miscCount)*(-2*(i%2) + 1));
		#define notesPerChord 4
		const double pitchIntervals[notesPerChord] = {0.0, 4.0, 7.0, 9.0}; // sixth chord
		const double speed = pow(semitoneRatio, 12*(i/notesPerChord) + pitchIntervals[i%notesPerChord]);
		setOscIncFromSpeed(i, vo_wave, speed);
		SDL_Delay(500);
	}
}

void ropeSwoosh(void) {
	disableVoices(voice_ropeSwooshL, voice_ropeSwooshR);
	voice v = {
		// shape,              amp, shift, pos, inc
		{  shape_ropeSwooshL, 16.0, 0.0,   0.0, 0.0 }, // wave
		{  shape_oneOne,       1.0, 0.0,   0.0, 0.0 }, // ampMod
		{  shape_oneOne,       1.0, 0.0,   0.0, 0.0 }, // spdMod
		{  shape_squareWav,    0.5, 0.5,   0.0, 0.0 }, // ampEnv
		{  shape_oneOne,       1.0, 0.0,   0.0, 0.0 }  // spdEnv
	};
	setVoice(voice_ropeSwooshL, v);
	v[vo_wave].shape = shape_ropeSwooshR;
	setVoice(voice_ropeSwooshR, v);
	setVoicePan(voice_ropeSwooshL, -1.0);
	setVoicePan(voice_ropeSwooshR,  1.0);
	setOscIncFromSpeed(voice_ropeSwooshL, vo_wave, 1.0);
	setOscIncFromSpeed(voice_ropeSwooshR, vo_wave, 1.0);
	setOscIncFromPeriod(voice_ropeSwooshL, vo_ampEnv, 4.8); // twice the period, putting it on the high part of the squarewave
	setOscIncFromPeriod(voice_ropeSwooshR, vo_ampEnv, 4.8);
	enableVoices(voice_ropeSwooshL, voice_ropeSwooshR);
}


int main(int argc, char **argv) {
	SDL_Init(SDL_INIT_TIMER);_sdlec;
	initVoices(voice_COUNT, shape_COUNT);
	float oneOne[1] = {1};
	shapeFromMem(shape_oneOne, 1, oneOne);
	shapeFromSine (shape_sineWav, 1024);
	shapeFromSaw  (shape_sawWav,  1024);
	shapeFromPulse(shape_squareWav, 2, 0.5);
	shapeFromPulse(shape_pulseWav,  2, 0.5);
	shapesFromWavFile(shape_goodEvening, 1, "GoodEveningRadioAudience.wav");
	shapesFromWavFile(shape_hellooo, 1, "Hellooo.wav");
	//shapesFromWavFile(shape_ropeSwooshL, 2, "77938__benboncan__ropeswoosh-3.wav");
	
	puts("unpauseAudio()");
	unpauseAudio();
	
	// fadeIn();
	// SDL_Delay(1000); puts("");
	// pan();
	// SDL_Delay(1000); puts("");
	// modulate();
	// SDL_Delay(1000); puts("");
	// envelope();
	// SDL_Delay(4000); puts("");
	// manyVoices();
	// SDL_Delay(1000); puts("");
	// pulseWidthSweep();
	// SDL_Delay(1000); puts("");
	goodEvening();
	SDL_Delay(3000); puts("");
	//hellooo();
	//SDL_Delay(5000); puts("");
	//ropeSwoosh();
	//SDL_Delay(3000); puts("");
	//restartVoices(voice_ropeSwooshL, voice_ropeSwooshR);
	
	closeVoices();
	SDL_Quit();_sdlec;
	return 0;
}

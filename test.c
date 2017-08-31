

#include <SDL2/SDL.h>
#include <math.h>
#include "voice.h"
#include "sdlec.h"

enum {
	voice_00,
	voice_COUNT
};
enum {
	shape_oneOne,
	shape_sineWav,
	shape_sawWav,
	shape_squareWav,
	shape_rectWav,
	shape_COUNT
};

int main(int argc, char **argv) {
	SDL_Init(SDL_INIT_TIMER);_sdlec;
	initVoices(voice_COUNT, shape_COUNT);
	{
		float oneOne[1] = {1};
		shapeFromMem(shape_oneOne, 1, oneOne);
	}
	shapeFromSine (shape_sineWav, 512);
	shapeFromSaw  (shape_sawWav,  512);
	shapeFromPulse(shape_squareWav, 2, 0.5);
	shapeFromPulse(shape_rectWav,  10, 0.3);
	
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
	
	puts("unpauseAudio()");
	unpauseAudio();
	SDL_Delay(1000);
	{
		float ama = 0.0;
		for (; ama < 0.24; ama += 0.005) {
			printf("setOscAmp(voice_00, vo_ampMod, %5.3f)\n", ama);
			setOscAmp(voice_00, vo_ampMod, ama);
			SDL_Delay(16);
		}
		SDL_Delay(1000);
		float sma = 0.0;
		for (; sma < 0.02; sma += 0.001) {
			printf("setOscAmp(voice_00, vo_incMod, %5.3f)\n", sma);
			setOscAmp(voice_00, vo_incMod, sma);
			SDL_Delay(16);
		}
		SDL_Delay(1000);
		for (; ama > 0.0; ama -= 0.005) {
			printf("setOscAmp(voice_00, vo_ampMod, %5.3f)\n", ama);
			setOscAmp(voice_00, vo_ampMod, ama);
			SDL_Delay(16);
		}
		SDL_Delay(1000);
		for (; sma > 0.0; sma -= 0.001) {
			printf("setOscAmp(voice_00, vo_incMod, %5.3f)\n", sma);
			setOscAmp(voice_00, vo_incMod, sma);
			SDL_Delay(16);
		}
		SDL_Delay(1000);
	}
	
	puts("setOscShape(voice_00, vo_ampEnv, shape_sawWav)");
	puts("setOscIncFromPeriod(voice_00, vo_ampEnv, 0.5)");
	setOscShape(voice_00, vo_ampEnv, shape_sawWav);
	setOscIncFromPeriod(voice_00, vo_ampEnv, 0.5); // stretch envelope to last 0.5 seconds
	
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(800);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(800);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(200);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(200);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(200);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(200);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(800);
	
	puts("setOscShape(voice_00, vo_incEnv, shape_sawWav)");
	puts("setOscIncFromPeriod(voice_00, vo_incEnv, 0.5)");
	setOscShape(voice_00, vo_incEnv, shape_sawWav);
	setOscIncFromPeriod(voice_00, vo_incEnv, 0.5);
	
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(800);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(800);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(200);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(200);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(200);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(200);
	puts("restartVoice(voice_00)");
	restartVoice(voice_00);
	SDL_Delay(800);
	
	
	
	// puts("disableVoice(voice_00)");
	// disableVoice(voice_00);
	// SDL_Delay(1000);
	
	// puts("enableVoice(voice_00)");
	// enableVoice(voice_00);
	// SDL_Delay(1000);
	
	// for (float v = 1.0; v >= 0.0; v -= 0.01) {
	// 	printf("setOscAmp(%4.2f)\n", v);
	// 	setGlobalVolume(v);
	// 	SDL_Delay(16);
	// }
	
	closeVoices();
	SDL_Quit();_sdlec;
	return 0;
}

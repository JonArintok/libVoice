

#include <SDL2/SDL.h>
#include <math.h>
#include "voice.h"
#include "sdlec.h"
#include "misc.h"

#define voiceCount 1
enum {
	shape_oneOne,
	shape_sineWav,
	shape_sineMod,
	shape_sawWav,
	shape_sawMod,
	shape_squareWav,
	shape_squareMod,
	shape_rectWav,
	shape_rectMod,
	shape_shapeCount
};

int main(int argc, char **argv) {
	SDL_Init(SDL_INIT_TIMER);_sdlec;
	initVoices(voiceCount, shape_shapeCount);
	{
		float oneOne[1] = {1};
		shapeFromMem(shape_oneOne, 1, oneOne);
	}
	shapeFromSine (shape_sineWav, 512, -1, 1);
	shapeFromSine (shape_sineMod, 512,  0, 1);
	shapeFromSaw  (shape_sawWav,  512, -1, 1);
	shapeFromSaw  (shape_sawMod,  512,  0, 1);
	shapeFromPulse(shape_squareWav, 2, -1, 1, 0.5);
	shapeFromPulse(shape_squareMod, 2,  0, 1, 0.5);
	shapeFromPulse(shape_rectWav,  10, -1, 1, 0.3);
	shapeFromPulse(shape_rectMod,  10,  0, 1, 0.3);
	syncShapes();
	{
		voice v = {
			{shape_sineWav, 1.0, 0.0, 0.0}, // wave
			{shape_oneOne,  1.0, 0.0, 1.0}, // ampMod
			{shape_oneOne,  1.0, 0.0, 1.0}, // spdMod
			{shape_oneOne,  1.0, 0.0, 1.0}, // ampEnv
			{shape_oneOne,  1.0, 0.0, 1.0}  // spdEnv
		};
		setVoice(0, v);
	}
	setOscIncFromLoopFreq(0, vo_wave, 440);
	
	puts("unpauseAudio()");
	unpauseAudio();
	SDL_Delay(1000);
	
	puts("disableVoice(0)");
	disableVoice(0);
	SDL_Delay(1000);
	
	puts("enableVoice(0)");
	enableVoice(0);
	SDL_Delay(1000);
	
	fr (i, 100) {
		float v = (100-i)/100.0;
		printf("setGlobalVolume(%4.2f)\n", v);
		setGlobalVolume(v);
		SDL_Delay(32);
	}
	fr (i, 100) {
		float v = i/100.0;
		printf("setGlobalVolume(%4.2f)\n", v);
		setGlobalVolume(v);
		SDL_Delay(32);
	}
	setGlobalVolume(1.0);
	SDL_Delay(1000);
	
	puts("closeVoices()");
	closeVoices();
	SDL_Quit();_sdlec;
	return 0;
}

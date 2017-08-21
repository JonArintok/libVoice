

#include <SDL2/SDL.h>
#include <math.h>
#include "voice.h"
#include "sdlec.h"

enum {
	voice_sine,
	voice_COUNT
};
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
	shape_COUNT
};

int main(int argc, char **argv) {
	SDL_Init(SDL_INIT_TIMER);_sdlec;
	initVoices(voice_COUNT, shape_COUNT);
	{
		float oneOne[1] = {1};
		shapeFromMem(shape_oneOne, 1, oneOne);
	}
	shapeFromSine (shape_sineWav, 512, -1,   1);
	shapeFromSine (shape_sineMod, 512,  0, 1);
	shapeFromSaw  (shape_sawWav,  512, -1,   1);
	shapeFromSaw  (shape_sawMod,  512,  0, 1);
	shapeFromPulse(shape_squareWav, 2, -1,   1, 0.5);
	shapeFromPulse(shape_squareMod, 2,  0,   1, 0.5);
	shapeFromPulse(shape_rectWav,  10, -1,   1, 0.3);
	shapeFromPulse(shape_rectMod,  10,  0,   1, 0.3);
	
	{
		voice v = {
			// 
			{shape_sineWav, 1.0, 0.0, 0.0}, // wave
			{shape_oneOne,  1.0, 0.0, 1.0}, // ampMod
			{shape_oneOne,  1.0, 0.0, 1.0}, // spdMod
			{shape_oneOne,  1.0, 0.0, 1.0}, // ampEnv
			{shape_oneOne,  1.0, 0.0, 1.0}  // spdEnv
		};
		setVoice(voice_sine, v);
	}
	setOscIncFromLoopFreq(voice_sine, vo_wave, 440);
	
	puts("unpauseAudio()");
	unpauseAudio();
	SDL_Delay(1000); puts("");
	
	puts("setOscIncFromLoopFreq(voice_sine, vo_ampMod, 2.0)");
	setOscIncFromLoopFreq(voice_sine, vo_ampMod, 4.0);
	puts("setOscShape(voice_sine, vo_ampMod, shape_sawMod)");
	setOscShape(voice_sine, vo_ampMod, shape_sawMod);
	puts("setOscAmp(voice_sine, vo_ampMod, 0.5)");
	setOscAmp(voice_sine, vo_ampMod, 0.5);
	SDL_Delay(2000); puts("");
	
	puts("setOscIncFromLoopFreq(voice_sine, vo_ampMod, 2.0)");
	setOscIncFromLoopFreq(voice_sine, vo_incMod, 8.0);
	puts("setOscShape(voice_sine, vo_ampMod, shape_sawMod)");
	setOscShape(voice_sine, vo_ampMod, shape_sawMod);
	SDL_Delay(2000); puts("");
	
	
	puts("disableVoice(voice_sine)");
	disableVoice(voice_sine);
	SDL_Delay(1000); puts("");
	
	puts("enableVoice(voice_sine)");
	enableVoice(voice_sine);
	SDL_Delay(1000); puts("");
	
	for (float v = 1.0; v >= 0.0; v -= 0.01) {
		printf("setGlobalVolume(%4.2f)\n", v);
		setGlobalVolume(v);
		SDL_Delay(16);
	}
	puts("closeVoices()");
	closeVoices();
	SDL_Quit();_sdlec;
	return 0;
}

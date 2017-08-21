

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
			{  shape_sineWav, 0.0,   1.0,  0.0, 0.0 }, // wave
			{  shape_oneOne,  0.0,   0.75, 0.0, 1.0 }, // ampMod
			{  shape_oneOne,  0.0,   1.0,  0.0, 1.0 }, // spdMod
			{  shape_oneOne,  0.0,   1.0,  0.0, 1.0 }, // ampEnv
			{  shape_oneOne,  0.0,   1.0,  0.0, 1.0 }  // spdEnv
		};
		setVoice(voice_sine, v);
	}
	setOscIncFromLoopFreq(voice_sine, vo_wave, 440);
	
	puts("unpauseAudio()");
	unpauseAudio();
	SDL_Delay(1000); puts("");
	
	puts(
		"setOscIncFromLoopFreq(voice_sine, vo_ampMod, 4.0);\n"
		"setOscShape(voice_sine, vo_ampMod, shape_sawWav);\n"
		"setOscShift(voice_sine, vo_ampMod, 0.75);"
	);
	setOscIncFromLoopFreq(voice_sine, vo_ampMod, 5.0); // pulse 5 times per second
	setOscShape(voice_sine, vo_ampMod, shape_sawWav);
	setOscShift(voice_sine, vo_ampMod, 0.75);
	for (float a = 0.0; a <= 0.24; a += 0.005) {
		printf("setOscAmp(%4.2f)\n", a);
		setOscAmp(voice_sine, vo_ampMod, a);
		SDL_Delay(16);
	}
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



#include <SDL2/SDL.h>
#include "voice.h"
#include "sdlec.h"
#include "math.h"

#define voiceCount 1
enum {
	shape_sin,
	shape_tri,
	shape_saw,
	shape_pulse,
	shape_shapeCount
};

int main(int argc, char **argv) {
	SDL_Init(SDL_INIT_TIMER);_sdlec;
	initVoices(voiceCount, shape_shapeCount);
	unpauseAudio();
	
	SDL_Delay(50);
	
	closeVoices();
	SDL_Quit();_sdlec;
	return 0;
}

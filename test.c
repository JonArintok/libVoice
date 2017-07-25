

#include <SDL2/SDL.h>
#include "voice.h"
#include "sdlec.h"

#define voiceCount 4
enum {
	shape_square,
	shape_saw,
	shape_tri,
	shape_sine,
	shape_shapeCount
};

int main(int argc, char **argv) {
	initVoices(voiceCount, shape_shapeCount);
	SDL_Init(SDL_INIT_TIMER);_sdlec;
	//
	//unpauseAudio();
	SDL_Delay(100);
	closeVoices();
	
	SDL_Quit();_sdlec;
	return 0;
}

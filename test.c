

#include <SDL2/SDL.h>
#include "voice.h"
#include "sdlec.h"


int main(int argc, char **argv) {
	initVoices(4);
	SDL_Init(SDL_INIT_TIMER);_sdlec
	//
	//unpauseAudio();
	SDL_Delay(100);
	closeVoices();
	return 0;
}

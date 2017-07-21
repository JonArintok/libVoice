

#include <SDL2/SDL.h>
#include "voice.h"
#include "sdlec.h"

typedef struct {
	long data[5];
} fiveLongArrayStruct;
typedef long fiveLongArray[5];

int main(int argc, char **argv) {
	
	printf("sizeof(fiveLongArray)       : %li\n", sizeof(fiveLongArray));
	printf("sizeof(fiveLongArrayStruct) : %li\n", sizeof(fiveLongArrayStruct));
	
	fiveLongArrayStruct flaS[2] = {
		{1, 2, 3, 4, 5},
		{10, 20, 30, 40, 50}
	};
	fiveLongArray flaA[2] = {
		{1, 2, 3, 4, 5},
		{10, 20, 30, 40, 50}
	};
	printf("flaS[1].data[4]   : %li\n", flaS[1].data[4]);
	printf("flaA[1][4]        : %li\n", flaA[1][4]);
	printf("((long*)flaA)[10] : %li\n", ((long*)flaA)[9]);
	
	
	/*
	initVoices(4);
	SDL_Init(SDL_INIT_TIMER);_sdlec
	//
	//unpauseAudio();
	SDL_Delay(100);
	closeVoices();
	*/
	return 0;
}

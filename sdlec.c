
#include <stdio.h>
#include <SDL2/SDL_error.h>

void sdlec(int line, const char *file) {
	const char *error = SDL_GetError();
	if (!error || !error[0]) return;
	printf("SDL error at line %i in %s :\n%s\n", line, file, error);
	SDL_ClearError();
}

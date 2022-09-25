#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <ecs.h>
#include <SDL.h>
#include <IMGUI.h>
#include "debug.h"
#include "adb.h"

#define CURRENT_TIME() (float)(clock() / CLOCKS_PER_SEC)

#define INLINE static inline

SDL_Window* gameWindow;
SDL_Renderer* gameRenderer;
int tryQuit;

// FORWARD DECL FUNC
INLINE void init();
INLINE void run();
INLINE void clean();

void system_events(ecsEntityId*, ecsComponentMask*, size_t, float);
void system_render(ecsEntityId*, ecsComponentMask*, size_t, float);

extern void register_systems();
extern void register_components();
extern void init_game();
extern void exit_game();

int main(/*int argc, char* argv[]*/)
{
	init();
	run();
	clean();

	return 0;
}

INLINE void init()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		exit(1);
	}

	if(SDL_CreateWindowAndRenderer(800, 420, 0, &gameWindow, &gameRenderer))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize window or renderer: %s\n", SDL_GetError());
		exit(2);
	}

	adbInit(101000);
	ecsInit();

	IM_Init();
	IM_SetRenderer(gameRenderer);

	register_components();

	ecsEnableSystem(system_events, nocomponent, ECS_NOQUERY);
	ecsEnableSystem(system_render, nocomponent, ECS_NOQUERY);

	register_systems();

	ecsRunTasks();

	init_game();
}

INLINE void run()
{
	const float targetTime = 1/60;
	float time = CURRENT_TIME(), lastTime;
	tryQuit = 0;

	SDL_RaiseWindow(gameWindow);

	while(!tryQuit)
	{
		lastTime = time;
		time = CURRENT_TIME();
		ecsRunSystems(time - lastTime);

		while(time < targetTime){time = CURRENT_TIME(); }
	}
}

INLINE void clean()
{
	exit_game();

	IM_Quit();
	ecsTerminate();
	adbTerminate();

	SDL_DestroyWindow(gameWindow);
	SDL_DestroyRenderer(gameRenderer);
	SDL_Quit();
}

void system_render(ecsEntityId* entities, ecsComponentMask* components, size_t count, float deltaTime)
{
	SDL_SetRenderDrawColor(gameRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gameRenderer);
	SDL_RenderPresent(gameRenderer);
	for(size_t i = 0; i < count; i++) {}
}

void system_events(ecsEntityId* entities, ecsComponentMask* components, size_t count, float deltaTime)
{
	SDL_Event evt;
	while(SDL_PollEvent(&evt))
	{
		switch(evt.type)
		{
		case SDL_QUIT:
			tryQuit = 1;
			break;
		}
	}
}


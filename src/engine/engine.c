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

int argCount;
char** argVector;

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

int main(int argc, char* argv[])
{
	argCount = argc;
	argVector = argv;
	init();
	run();
	clean();

	return 0;
}

/*
 * find the lexical parent path to path and output it into outPath and its length into outLen.
 * this function does not check if its input is a valid path; it only searches for the first '/' not at the end of the path.
 */
INLINE void findLexicalParentPath(char* path, char** outPath, size_t* outLen)
{
	size_t startLength = strlen(path);
	size_t length;
	
	/* -2 to avoid picking the current directory */
	char* c = path + startLength - 2;
	/* find the parent directory */
	for(; c != path; c--)
	{
		if(*c == '/')
		{
			break;
		}
	}
	/* return if no parent directory was found */
	if(*c != '/')
	{
		*outPath = NULL;
		*outLen = 0;
		return;
	}
	else
	{
		length = c - path + 1;
		char* npath = malloc((length) * sizeof(char));
		if(npath == NULL)
		{
			*outPath = NULL;
			*outLen = 0;
			return;
		}
		*outPath = npath;
		*outLen = length;
		memcpy(npath, path, (length-1) * sizeof(char));
		npath[length-1] = '\0';
	}
}

INLINE void set_default_asset_path()
{
	static const char* relativeAssetsPath = "../Assets/";

	char* execParentDir;
	size_t execParentDirLen;
	findLexicalParentPath(argVector[0], &execParentDir, &execParentDirLen);

	DEBUG_LOGLN("dir len = %zu, dir path = %s", execParentDirLen, execParentDir);
	adbSetAssetRoot(execParentDir, execParentDirLen);
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

	adbInit(100);
	set_default_asset_path();

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


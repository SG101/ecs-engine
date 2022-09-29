#include <ecs.h>
#include <adb.h>
#include <debug.h>
#include <IMGUI.h>

SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font* font;
IM_TextStyle style;
IM_SlicedImage defaultSliced;
IM_ButtonStyle button;

void system_logMessage(ecsEntityId* entities, ecsComponentMask* components, size_t count, float deltaTime)
{ }

void system_renderSecondWindow(ecsEntityId* entities, ecsComponentMask* components, size_t count, float deltaTime)
{
	SDL_Rect rect = {0,0, 250, 15};
	IM_SetRenderer(renderer);

	if(IM_Button(rect, &button, "Button"))
	{
		DEBUG_LOGLN("Triggered button");
	}

	IM_Render();
}

void register_systems()
{
	DEBUG_LOGLN("register_systems");
//	ecsEnableSystem(system_logMessage, nocomponent, ECS_NOQUERY);
	ecsEnableSystem(system_renderSecondWindow, nocomponent, ECS_NOQUERY);
}

void register_components()
{
	DEBUG_LOGLN("register_components");
}

void init_game()
{
	DEBUG_LOGLN("game_init");
	if(SDL_CreateWindowAndRenderer(250, 500, 0, &window, &renderer) < 0)
	{
		DEBUG_LOGLN("SDL error when creating secondary window: %s", SDL_GetError());
	}

	IM_SlicedImage btnimg = {
		.surface=adbGetSurface("NineSliceA"),
	};

	button.image = btnimg;

	IM_SetRenderer(renderer);
}

void exit_game()
{
	DEBUG_LOGLN("exit_game");
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

#include <ecs.h>
#include <debug.h>

void system_logMessage(ecsEntityId* entities, ecsComponentMask* components, size_t count, float deltaTime)
{
	//DEBUG_LOGLN("YAY!!");
}

void register_systems()
{
	DEBUG_LOGLN("register_systems");
	ecsEnableSystem(system_logMessage, nocomponent, ECS_NOQUERY);
}

void register_components()
{
	DEBUG_LOGLN("register_components");
}

void init_game()
{
	DEBUG_LOGLN("game_init");
}

void exit_game()
{
	DEBUG_LOGLN("exit_game");
}

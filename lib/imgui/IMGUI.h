//
//  IMGUI.h
//  sdlimgui
//
//  Created by Scott on 25/08/2022.
//

#ifndef IMGUI_hpp
#define IMGUI_hpp

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "IMGUI_def.h"

#if __cplusplus
extern ( "C" )
{
#endif

	//
	// UTILITY FUNCTIONS
	//
	extern int		IM_ValidAlignment	( IM_Alignment align);
	extern int		IM_RectContains		( SDL_Rect r1, int x, int y );
	
	
	//
	//	SYSTEM FUNCTIONS
	//
	
	/**
	 * \brief Initialize the IMGUI backend
	 * \returns 0 if no errors occured
	 */
	extern int		IM_Init				( void );
	/**
	 * \brief Cleans up the IMGUI backend
	 */
	extern void		IM_Quit				( void );
	/**
	 * \brief Initialize drawing onto the given target.
	 */
	extern void		IM_SetRenderer		( SDL_Renderer* target );
	extern void		IM_PushTarget		( SDL_Texture* target );
	extern void		IM_PopTarget		( void );
	/**
	 * \brief Clean up after frame and present the drawn frame
	 */
	extern void		IM_Render			( void );
	/**
	 * \brief Wait for the next system event.
	 * Functions as a wrapper function for SDL_WaitEvent.
	 */
	extern int 		IM_WaitEvent		( SDL_Event* evt );
	extern void		IM_ProcessEvent		( SDL_Event evt );
	/**
	 * \brief Set whether or not to show debug info.
	 */
	extern void		IM_ShowDebugInfo	( int value );
	/**
	 * \returns 1 if IMGUI is set to show debug info.
	 */
	extern int		IM_IsDebugMode		( void );
	
	
	//
	// DRAWING FUNCTIONS
	//
	
	extern void		IM_RectOutline		( SDL_Rect rect, const SDL_Colour* fill, const SDL_Colour* outline, unsigned outlineThickness );
	extern void		IM_Rect				( SDL_Rect rect, const SDL_Colour* fill );
	extern void		IM_Image			( SDL_Rect rect, SDL_Rect srcRect, SDL_Surface* image, const SDL_Colour* tint );
	extern void		IM_Sliced			( SDL_Rect rect, IM_SlicedImage* image, const SDL_Colour* tint );
	extern void		IM_Text				( SDL_Rect rect, const char* str, const IM_TextStyle* style );
	extern int		IM_Button			( SDL_Rect rect, IM_ButtonStyle* button, const char* label );
	extern int		IM_Toggle			( SDL_Rect rect, IM_ButtonStyle* button, const char* label, int* value );
	extern double	IM_Slider			( SDL_Rect rect, IM_ButtonStyle* button, double* value );
	
#if __cplusplus
}
#endif

#endif /* IMGUI_h */

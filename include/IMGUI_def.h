//
//  IMGUI_defs.h
//  sdlimgui
//
//  Created by Scott on 26/08/2022.
//

#ifndef _IMGUI_def_hpp
#define _IMGUI_def_hpp

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#if __cplusplus
extern ( "C" )
{
#endif

	// alignment of text from the left/right top/bottom edge or centered in either
	// bitmask with the first 4 bits reserved for horizontal alignment
	// and the next 4 reserved for vertical alignment
	typedef enum IM_Alignment
	{
		IM_ALIGN_H_CENTER	= 0x01, // bit 1
		IM_ALIGN_LEFT		= 0x02, // bit 2
		IM_ALIGN_RIGHT		= 0x04, // bit 3
		
		IM_ALIGN_V_CENTER	= 0x10, // bit 5
		IM_ALIGN_TOP		= 0x20, // bit 6
		IM_ALIGN_BOTTOM		= 0x40, // bit 7
		
		IM_ALIGN_CENTER		= IM_ALIGN_H_CENTER | IM_ALIGN_V_CENTER,
	} IM_Alignment;
	
	// style struct for passing to IM_Text(...)
	typedef struct IM_TextStyle
	{
		IM_Alignment	alignment;
		SDL_Colour		colour;
		TTF_Font*		font;
	} IM_TextStyle;
	
	// data needed to render a sliced image through IM_Sliced(...)
	typedef struct IM_SlicedImage
	{
		SDL_Surface*	surface;
		SDL_Rect		slices;
		int				cornerWidth;
		int				cornerHeight;
	} IM_SlicedImage;
	
	
	// style struct for passing to IM_Button(...)
	typedef struct IM_ButtonStyle
	{
		IM_SlicedImage	image;				// the background image
		IM_TextStyle	text;				// the style to render the label in
		SDL_Colour		inactive;			// the colour when idle
		SDL_Colour		active;				// the colour when hovered
		SDL_Colour		clicked;			// the colour when clicked
	} IM_ButtonStyle;
	
	
	typedef struct IM_Panel
	{
		SDL_Texture*	panel;
		SDL_Rect		rect;
	} IM_Panel;
	

#if __cplusplus
}
#endif

#endif // !_IMGUI_def_hpp

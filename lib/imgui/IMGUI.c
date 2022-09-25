//
//  IMGUI.c
//  sdlimgui
//
//  Created by Scott on 25/08/2022.
//

#include "IMGUI.h"
#include "IMGUI_TextureCache.h"
#include <stdint.h>
#include <memory.h>
#include <math.h>

SDL_Renderer* _IM_Renderer = NULL;
SDL_Event _IM_Event;

Uint8 _IM_MouseButtonState = 0;
Uint8 _IM_MouseButtonsChanged = 0;

int _IM_ShowDebug = 0;

/// UTILITIES

size_t IM_HashStr ( const char* str )
{
	// calculate the number of shifts between the first and last 4 bits of hash
	static const int shift = ( 2 * sizeof ( size_t ) + 4 );
	// calculate a mask encompassing the last four bits of an unsigned value
	static const size_t upper4mask = 0xF << shift;
	
	size_t hash = 0;
	size_t upper4 = 0;
	size_t l = 0;
	
	for ( const char* c = str; *c != '\0'; c++ )
	{
		upper4 = hash & upper4mask;
		hash = ( ( hash << 4 ) ^ ( *c ) );
		if ( upper4 != 0 )
			hash ^= ( upper4 >> shift );
		l++;
	}
	
	return hash ^ l;
}

SDL_Texture* IM_GetTextureFor ( SDL_Surface* surface )
{
	SDL_Texture* found = TC_GetCachedTexture ( surface );
	if ( found == NULL )
		found = SDL_CreateTextureFromSurface ( _IM_Renderer, surface );
	return found;
}

//
//	Check if the given alignment is valid.
//	Validity is defined as having only one bitmask entry per axis.
//
int IM_ValidAlignment ( IM_Alignment align )
{
	int nv = 0, nh = 0;
	
	// count number of vertical alignments
	if ( ( align & IM_ALIGN_TOP ) != 0 )
		nv++;
	if ( ( align & IM_ALIGN_BOTTOM ) != 0 )
		nv++;
	if ( ( align & IM_ALIGN_V_CENTER ) != 0 )
		nv++;
	
	// count number of horizontal alignments
	if ( ( align & IM_ALIGN_LEFT ) != 0 )
		nh++;
	if ( ( align & IM_ALIGN_RIGHT ) != 0 )
		nh++;
	if ( ( align & IM_ALIGN_H_CENTER ) != 0 )
		nh++;
	
	// return true if there is exactly one alignment on each axis
	return ( nv == 1 && nh == 1 );
}

//
//	Checks if point (x, y) is contained within rectangle r
//	NOTE: both position and size are inclusive ( (r.x,r.y) and (r.x+r.w, r.y+r.h) both return 1 )
//
int IM_RectContains ( SDL_Rect r, int x, int y )
{
	return ( x >= r.x && x <= r.x+r.w ) && ( y >= r.y && y <= r.y+r.h );
}

/// MANAGEMENT FUNCTIONS

//
// Initialize IMGUI context to render to the given window
//
int IM_Init ( )
{
	TC_Init ( );
	return 0;
}

//
//	Clean up any memory allocated for IMGUI
//
void IM_Quit ( void )
{
	if ( _IM_Renderer != NULL )
		SDL_DestroyRenderer ( _IM_Renderer );
	
	TC_Quit ( );
}

//
//	Wrapper for SDL_PollEvent which processes particular events for IMGUI
//
int IM_WaitEvent ( SDL_Event* evt_ptr )
{
	int r = SDL_WaitEventTimeout ( &_IM_Event, 500);

	if(r)
	{
		IM_ProcessEvent ( _IM_Event );
	}
	
	*evt_ptr = _IM_Event;
	return r;
}

void IM_ProcessEvent ( SDL_Event evt )
{
	_IM_MouseButtonsChanged = 0;	
	switch ( _IM_Event.type )
	{
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
		{
			_IM_MouseButtonsChanged |= _IM_Event.button.button;
			_IM_MouseButtonState = SDL_GetMouseState(NULL, NULL);
		}
	}
}

//
//	Get IMGUI ready to draw the next frame
//
void IM_SetRenderer ( SDL_Renderer* target )
{
	if ( target != _IM_Renderer )
		TC_ClearTextureCache ( );
	_IM_Renderer = target;
	
	SDL_SetRenderDrawColor ( _IM_Renderer, 0, 0, 0, 255 );
	SDL_RenderClear ( _IM_Renderer );
	SDL_SetRenderDrawBlendMode ( _IM_Renderer, SDL_BLENDMODE_BLEND );
}

//
//	Clean up and present a renderered frame
//
void IM_Render ( )
{
	SDL_RenderPresent ( _IM_Renderer );
	SDL_SetRenderDrawColor ( _IM_Renderer, 0, 0, 0, 255 );
	SDL_RenderClear ( _IM_Renderer );
	SDL_SetRenderDrawBlendMode ( _IM_Renderer, SDL_BLENDMODE_BLEND );
}

//
//	Set whether or not to show debug info
//
void IM_ShowDebugInfo ( int value )
{
	_IM_ShowDebug = value;
}

//
//	Returns whether or not debug info is shown
//
int IM_IsDebugMode ( )
{
	return _IM_ShowDebug;
}


/// DRAWING FUNCTIONS

//
//	Render a rectangle for this frame.
//
void IM_Rect ( SDL_Rect rect, const SDL_Colour* colour )
{
	SDL_SetRenderDrawColor ( _IM_Renderer,
							colour->r, colour->g, colour->b,
							colour->a );

	if( SDL_RenderFillRect ( _IM_Renderer, &rect) != 0 )
	{
		SDL_Log ( "IM_Rect: %s", SDL_GetError() );
		return;
	}
}

//
//	Render a rectangle with an outline for this frame.
//
void IM_RectOutline ( SDL_Rect rect,
					 const SDL_Colour* fill, const SDL_Colour* outline,
					 unsigned outlineThickness )
{
	IM_Rect ( rect, fill );
	
	if( outline->a <= 0 ) return;
	SDL_SetRenderDrawColor ( _IM_Renderer,
							outline->r, outline->g, outline->b,
							outline->a );
	while ( outlineThickness-- > 0 )
	{
		SDL_RenderDrawRect ( _IM_Renderer, &rect );
		rect.x++; rect.y++;
		rect.w -= 2; rect.h -= 2;
	}
}

//
//	Render the given string in the given style.
//
void IM_Text ( SDL_Rect rect, const char* str, const IM_TextStyle* style )
{
	SDL_Surface* surface = NULL;
	SDL_Texture* texture = NULL;
	SDL_Rect dstRect;
	size_t hash = 0;
	
	if ( IM_ValidAlignment ( style->alignment ) == 0 )
	{
		SDL_Log ( "IM_Text: Text style has invalid alignment 0x%X.", ( style->alignment ) );
		return;
	}
	
	hash = IM_HashStr ( str );
	texture = TC_GetCachedTexture ( ( void* ) hash );
	if ( texture == NULL )
	{
		// generate texture from the text
		surface = TTF_RenderText_Solid_Wrapped ( style->font, str, style->colour, rect.w );
		texture = SDL_CreateTextureFromSurface ( _IM_Renderer, surface );
		SDL_FreeSurface ( surface ); // destroy surface once no longer needed
	}
	dstRect = ( SDL_Rect ) { .x = rect.x, .y = rect.y };
	
	// load texture width/height into dstRect
	SDL_QueryTexture ( texture, NULL, NULL, &dstRect.w, &dstRect.h );
	
	rect.x = 0;
	rect.y = 0;
	
	if ( dstRect.h > rect.h )
		dstRect.h = rect.h;
	if ( dstRect.w > rect.w )
		dstRect.w = rect.w;
	
	if ( style->alignment & IM_ALIGN_RIGHT )
		dstRect.x += rect.w - dstRect.w;
	else if ( style->alignment & IM_ALIGN_V_CENTER )
		dstRect.y += ( rect.h >> 1 ) - ( dstRect.h >> 1 );
	
	if ( style->alignment & IM_ALIGN_BOTTOM )
		dstRect.y += rect.h - dstRect.h;
	else if ( style->alignment & IM_ALIGN_H_CENTER )
		dstRect.x += ( rect.w >> 1 ) - ( dstRect.w >> 1 );

	SDL_RenderCopy ( _IM_Renderer, texture, &rect, &dstRect );
	
	if ( TC_CacheTexture ( ( void* ) hash, texture ) == 0 )
		SDL_DestroyTexture ( texture );
}

//
//	Draw an image for this frame.
//
void IM_Image ( SDL_Rect rect, SDL_Rect srcRect, SDL_Surface* image, const SDL_Colour* tint )
{
	SDL_Texture* tex;
	
	tex = SDL_CreateTextureFromSurface ( _IM_Renderer, image );
	
	if ( tint == NULL )
		SDL_SetTextureColorMod ( tex, 255, 255, 255 );
	else
		SDL_SetTextureColorMod ( tex, tint->r, tint->g, tint->b );
	
	SDL_RenderCopy ( _IM_Renderer, tex, &srcRect, &rect );
	
	if ( TC_CacheTexture ( image, tex ) == 0)
		SDL_DestroyTexture ( tex );
}

//
//	Draw a 9-sliced image.
//
void IM_Sliced ( SDL_Rect rect, IM_SlicedImage* image, const SDL_Colour* tint )
{
	SDL_Texture* tex;
	SDL_Rect srcRect;
	SDL_Rect dstRect;
	
	int cw = image->cornerWidth, ch = image->cornerHeight;
	
	// avoid overlapping corners
	if ( rect.w < ( cw << 1 ) )
		cw = rect.w >> 1;
	if ( rect.h < ( ch << 1 ) )
		ch = rect.h >> 1;
	
	// set starting values for source/destination rectangles
	srcRect = ( SDL_Rect ) { 0, 0, 0, 0 };
	dstRect = ( SDL_Rect ) { rect.x, rect.y, 0, 0 };
	
	// precalculate source rectangle sizes
	const int xSrc[3] =
	{ ( image->slices.x ), ( image->slices.w ), ( image->surface->w ) - ( image->slices.x) - ( image->slices.w ) };
	const int ySrc[3] =
	{ ( image->slices.y ), ( image->slices.h ), ( image->surface->h ) - ( image->slices.y) - ( image->slices.h ) };
	// precalculate destination rectangle sizes
	const int xDst[3] =
	{ ( cw ), rect.w - ( ( cw ) << 1 ), ( cw ) };
	const int yDst[3] =
	{ ( ch ), rect.h - ( ( ch ) << 1), ( ch ) };
	
	tex = IM_GetTextureFor ( image->surface );
	
	if ( tint == NULL )
		SDL_SetTextureColorMod ( tex, 255, 255, 255 );
	else
		SDL_SetTextureColorMod ( tex, tint->r, tint->g, tint->b );
	
	if ( _IM_ShowDebug )
	{
		SDL_SetRenderDrawColor ( _IM_Renderer, 0, 255, 0, 255 );
		SDL_RenderDrawRect ( _IM_Renderer, &rect );
	}
	
	int x = 0, y = 0;
	while ( y < 3 )
	{
		srcRect.h = ySrc[y];
		dstRect.h = yDst[y];
		
		while ( x < 3 )
		{
			srcRect.w = xSrc[x];
			dstRect.w = xDst[x];
			
			SDL_RenderCopy ( _IM_Renderer, tex, &srcRect, &dstRect );
			
			if ( _IM_ShowDebug )
			{
				SDL_SetRenderDrawColor ( _IM_Renderer, 0, 255, 0, 255 );
				SDL_RenderDrawRect ( _IM_Renderer, &dstRect );
			}
			
			srcRect.x += srcRect.w;
			dstRect.x += dstRect.w;
			
			x++;
		}
		x = 0;
		srcRect.x = 0;
		dstRect.x = rect.x;
		
		srcRect.y += srcRect.h;
		dstRect.y += dstRect.h;
		
		y++;
	}
	
	if ( TC_CacheTexture ( image->surface, tex ) == 0)
		SDL_DestroyTexture ( tex );
}

//
//	Draw a clickable button.
//
int IM_Button ( SDL_Rect rect, IM_ButtonStyle* style, const char* label )
{
	int interaction = 0, mx, my;
	const SDL_Colour* fillColour;
	
	SDL_GetMouseState ( &mx, &my );
	fillColour = &( style->inactive );
	
	if( IM_RectContains ( rect, mx, my ) )
	{
		interaction = 1;
		fillColour = &( style->active );
		if ( ( _IM_MouseButtonsChanged & _IM_MouseButtonState & SDL_BUTTON_LMASK ) != 0 )
		{
			fillColour = &( style->clicked );
			interaction = 2;
		}
	}
	
	IM_Sliced( rect, ( IM_SlicedImage* ) style, fillColour );
	IM_Text ( rect, label, &( style->text ) );
	
	return interaction >= 2;
}

int IM_Toggle ( SDL_Rect rect, IM_ButtonStyle* style, const char* label, int* value )
{
	int interaction = 0, mx, my;
	const SDL_Colour* fillColour;
	
	SDL_GetMouseState( &mx, &my );
	fillColour = &( style->active );
	
	if ( IM_RectContains ( rect, mx, my ) )
	{
		interaction = 1;
		fillColour = &( style->active );
		if ( ( _IM_MouseButtonsChanged & _IM_MouseButtonState & SDL_BUTTON_LMASK ) != 0 )
		{
			fillColour = &( style->clicked );
			*value = !(*value);
			interaction = 2;
		}
	}
	
	if ( *value )
	{
		fillColour = &( style->clicked );
	}
	
	IM_Sliced ( rect, ( IM_SlicedImage* ) style, fillColour );
	IM_Text ( rect, label, &( style->text ) );
	
	return interaction >= 2;
}

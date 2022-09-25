//
//  IMGUI_TextureCache.c
//  sdlimgui
//
//  Created by Scott on 27/08/2022.
//

#include "IMGUI_TextureCache.h"

// key-value-pair for texture cache ( key is ideally the SDL_Surface* the SDL_Texture was created from )
typedef struct { void* key; SDL_Texture* value; } IM_TextureCacheEntry;
// cache
IM_TextureCacheEntry* _IM_TextureCache = NULL;

// relevant offsets
unsigned _IM_TextureCacheAllocated = 0;
unsigned _IM_TextureCacheUsed = 0;

const char* _TC_NotInitMsg = "'Attempted to access texture cache before starting it with TC_Init'";


int TC_AllocateTextureCache ( unsigned size )
{
	if ( _IM_TextureCache != NULL )
		return 0;
	
	_IM_TextureCache = malloc( sizeof ( IM_TextureCacheEntry ) * size);
	
	// update texture cache size
	if ( _IM_TextureCache == NULL )
		_IM_TextureCacheAllocated = 0;
	else
		_IM_TextureCacheAllocated = size;
	
	return _IM_TextureCacheAllocated;
}

void TC_FreeTextureCache ( )
{
	free ( _IM_TextureCache );
	_IM_TextureCache = NULL;
}

void TC_Init ( )
{
	TC_AllocateTextureCache ( 8 );
}

void TC_Quit ( )
{
	TC_ClearTextureCache ( );
	TC_FreeTextureCache ( );
}

int TC_ResizeTextureCache ( unsigned size )
{
	if ( _IM_TextureCache == NULL )
	{
		SDL_Log ( "TC_ResizeTextureCache: %s", _TC_NotInitMsg );
		return 0;
	}
	
	// remove entries that would be lost on realloc
	if ( _IM_TextureCacheUsed > size )
	{
		for ( IM_TextureCacheEntry* ptr = ( _IM_TextureCache + size ); ptr != ( _IM_TextureCache + _IM_TextureCacheUsed ); ptr++)
			SDL_DestroyTexture ( ptr->value );
		_IM_TextureCacheUsed = size;
	}
	
	// attempt to resize cache
	IM_TextureCacheEntry* new = realloc ( _IM_TextureCache, sizeof ( IM_TextureCacheEntry ) * size );
	
	if ( new != NULL ) // realloc succeeded
	{
		_IM_TextureCache = new;
		_IM_TextureCacheAllocated = size;
	}
	
	return _IM_TextureCacheAllocated;
}

void TC_ClearTextureCache ( )
{
	if ( _IM_TextureCache == NULL )
	{
		SDL_Log ( "TC_ClearTextureCache: %s", _TC_NotInitMsg );
		return;
	}
	
	for ( IM_TextureCacheEntry* ptr = _IM_TextureCache; ptr != ( _IM_TextureCache + _IM_TextureCacheUsed ); ptr++ )
	{
		SDL_DestroyTexture ( ptr->value );
	}
	_IM_TextureCacheUsed = 0;
}

SDL_Texture* TC_GetCachedTexture ( void* key )
{
	if ( _IM_TextureCache == NULL )
	{
		SDL_Log ( "TC_GetCachedTexture: %s", _TC_NotInitMsg );
		return NULL;
	}
	
	for ( IM_TextureCacheEntry* ptr = _IM_TextureCache; ptr != ( _IM_TextureCache + _IM_TextureCacheUsed ); ptr++ )
	{
		if ( ptr->key == key )
		{
			return ptr->value;
		}
	}
	
	return NULL;
}

int TC_CacheTexture ( void* key, SDL_Texture* texture )
{
	int hasSpace;
	IM_TextureCacheEntry* ptr;
	
	if ( _IM_TextureCache == NULL )
	{
		SDL_Log ( "TC_CacheTexture: %s", _TC_NotInitMsg );
		return 0;
	}
	
	if ( TC_GetCachedTexture ( key ) != NULL)
		return 1;
	
	// calculate remaining cache space
	hasSpace = _IM_TextureCacheAllocated - _IM_TextureCacheUsed;
	
	// attempt to increase cache space as needed
	if ( hasSpace <= 0 )
	{
		// realloc and recalculate remaining space
		hasSpace = TC_ResizeTextureCache ( _IM_TextureCacheAllocated + 8 ) - _IM_TextureCacheUsed;
	}
	
	// store cache entry
	if( hasSpace > 0)
	{
		// get a pointer to the next cache entry
		ptr			= _IM_TextureCache + _IM_TextureCacheUsed;
		ptr->value	= texture;
		ptr->key	= key;
		_IM_TextureCacheUsed++;
		return 1;
	}
	return 0;
}

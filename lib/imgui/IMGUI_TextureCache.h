//
//  IMGUI_TextureCache.h
//  sdlimgui
//
//  Created by Scott on 27/08/2022.
//

#ifndef IMGUI_TextureCache_h
#define IMGUI_TextureCache_h

#include <SDL.h>

#if __cplusplus
extern ( "C" )
{
#endif
	
	/**
	 * \brief Initializes and allocates texture cache.
	 */
	extern void				TC_Init						( void );
	/**
	 * \brief Clean up and deallocate memory related to the texture cache.
	 */
	extern void				TC_Quit						( void );
	/**
	 * \brief Attempts to reallocate the texture cache.
	 * \param size is the new size in number of textures that should be storable in the texture cache.
	 * \returns The size of the reallocated cache.
	 */
	extern int				TC_ResizeTextureCache		( unsigned size );
	/**
	 * \brief Deallocates all textures in the cache.
	 */
	extern void				TC_ClearTextureCache		( void );
	/**
	 * \brief Searches for a cached texture.
	 * \param key refers to the same key this texture was cached with.
	 * \returns a pointer to a found texture or NULL if none was found.
	 */
	extern SDL_Texture*		TC_GetCachedTexture			( void* key );
	/**
	 * \brief Cache a texture.
	 * \param key is the key to be used to find this texture again.
	 * \param texture is 	the texture to cache.
	 * \returns 1 if caching succeeded, 0 if caching failed.
	 */
	extern int				TC_CacheTexture				( void* key, SDL_Texture* texture );
	
#if __cplusplus
}
#endif

#endif /* IMGUI_TextureCache_h */

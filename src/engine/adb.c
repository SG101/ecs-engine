#include "adb.h"

#include <assert.h>
#include <memory.h>

typedef void(*adbDeleterFn)(void*);

typedef struct adbAsset {
	size_t pathhash;
	void* instance;
	adbDeleterFn deleter;
} adbAsset;

typedef struct adbTexture {
	SDL_Texture* texture;
	SDL_Renderer* createdFrom;
} adbTexture;

typedef struct adbSurface {
	adbTexture* textures;
	size_t textureCount;
	SDL_Surface* originSurface;
} adbSurface;

int adbIsInit = 0;
adbAsset* adbLoadedAssets;
size_t adbAssetCount;
size_t adbMaxAssets;

void adbSurfaceDelete(void* surface)
{
	adbSurface* inst = surface;

	for(size_t i = 0; i < inst->textureCount; ++i)
	{
		SDL_DestroyTexture(inst->textures[i].texture);
	}

	free(inst->textures);
	free(inst->originSurface);
	free(inst);
}

void adbInit(size_t maxassets)
{
	assert(!adbIsInit);
	adbMaxAssets = maxassets;

	adbLoadedAssets = malloc(maxassets*sizeof(adbAsset));
	memset(adbLoadedAssets, 0, maxassets*sizeof(adbAsset));

	adbAssetCount = 0;
	adbIsInit = 1;
}

void adbDeleteAssets()
{
	if(adbLoadedAssets != NULL)
	{
		for(size_t i = 0; i < adbAssetCount; ++i)
		{
			if(adbLoadedAssets[i].instance != NULL)
			{
				adbLoadedAssets[i].deleter(adbLoadedAssets[i].instance);
				adbLoadedAssets[i].instance = NULL;
			}
		}
		free(adbLoadedAssets);
	}
}

void adbTerminate()
{
	assert(adbIsInit);
	adbDeleteAssets();
}

void adbClear()
{
	adbDeleteAssets();
	adbInit(adbMaxAssets);
}

size_t adbHashStr ( const char* str )
{
	// calculate the number of shifts between the first and last 4 bits of hash
	static const int shift = ( 8 * sizeof ( size_t ) - 4 );
	// calculate a mask encompassing the last four bits of an unsigned value
	static const size_t upper4mask = (size_t)0xF << shift;
	
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

/*
 * \brief finds the first free bucket starting at hash.
 * \returns A) the index of the first free bucket starting at hash or B) adbMaxAssets if no free bucket was found before looping back to hash.
 */
size_t adbFindFreeBucket(size_t hash)
{
	size_t start = hash % adbMaxAssets;
	size_t i = start;
	do {
		if(adbLoadedAssets[i].pathhash == 0)
			return i;
		i = (i+1) % adbMaxAssets;
	} while(i != start);

	return adbMaxAssets;
}

/*
 * \brief finds the index refering to the asset with the pathhash of hash.
 * \returns the index of the asset with pathhash hash or adbMaxAssets if the asset was not found.
 */
size_t adbFindAsset(size_t hash)
{
	size_t start = hash % adbMaxAssets;
	size_t i = start;
	do {
		if(adbLoadedAssets[i].pathhash == hash)
			return i;
		i = (i+1) % adbMaxAssets;
	} while(i != start);

	return adbMaxAssets;
}

/*
 * \brief finds either the asset with the provided hash or the first free bucket if the asset is not loaded.
 */
size_t adbFindAssetOrFree(size_t hash)
{
	size_t start = hash % adbMaxAssets;
	size_t i = start;
	size_t firstFree = adbMaxAssets;
	do {
		if(adbLoadedAssets[i].pathhash == hash)
			return i;

		if(firstFree == adbMaxAssets && adbLoadedAssets[i].pathhash == 0)
			firstFree = i;
	} while(i != start);

	return firstFree;
}

TTF_Font* adbGetFont(const char* filepath)
{
	size_t hash = adbHashStr(filepath);

	size_t found = adbFindAssetOrFree(hash);

	if(found == adbMaxAssets)
		return NULL;

	// if the the found bucket is empty, load the asset from disk and store it
	if(adbLoadedAssets[found].pathhash == 0)
	{
		TTF_Font* file = TTF_OpenFont(filepath, 30);
		adbLoadedAssets[found].instance = file;
		adbLoadedAssets[found].pathhash = hash;
		adbLoadedAssets[found].deleter = &TTF_CloseFont;
		return file;
	}
	// if the found bucket is filled, return that asset
	else
	{
		return adbLoadedAssets[found].instance;
	}
	assert(0);
}

SDL_Texture* adbGetTexture(const char* filepath, SDL_Renderer* renderer)
{
	size_t hash = adbHashStr(filepath);
	size_t found = adbFindAssetOrFree(hash);
	if(found == adbMaxAssets)
		return NULL;

	if(adbLoadedAssets[found].pathhash == 0)
	{
		// load file
		SDL_Surface* surf = IMG_Load(filepath);

		// create a new instance of adbSurface in hash bucket
		adbLoadedAssets[found].instance = malloc(sizeof(adbSurface));
		adbLoadedAssets[found].deleter = &adbSurfaceDelete;

		// set initial values of created surface
		adbSurface* data = adbLoadedAssets[found].instance;
		data->originSurface = surf;
		data->textures = malloc(1*sizeof(adbTexture));
		data->textureCount = 1;

		// initialize requested texture
		data->textures[0].texture = SDL_CreateTextureFromSurface(renderer, surf);
		data->textures[0].createdFrom = renderer;

		return data->textures[0].texture;
	}
	else
	{
		// check if the texture exists already
		adbSurface* surface = adbLoadedAssets[found].instance;
		for(int i = 0; i < surface->textureCount; ++i)
		{
			if(surface->textures[i].createdFrom == renderer)
			{
				return surface->textures[i].texture;
			}
		}

		// attempt to reallocate, break on failure
		void* nptr = realloc(surface->textures, (surface->textureCount+1) * sizeof(adbTexture));
		assert(nptr != NULL);

		// set textures ptr to reallocated ptr
		surface->textures = nptr;

		// allocate a texture
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface->originSurface);

		// update the now available last element of the list
		adbTexture* store = &surface->textures[surface->textureCount-1];
		store->texture = texture;
		store->createdFrom = renderer;
	}

	assert(0);
}


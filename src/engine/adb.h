#ifndef _adb_h
#define _adb_h

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#ifdef __cplusplus
extern("C") {
#endif

extern void adbInit(size_t maxassets);
extern void adbTerminate();
extern void adbClear();

extern void adbSetAssetRoot(const char* assetRootPath, size_t len);

extern TTF_Font* adbGetFont(const char* filepath);
extern SDL_Texture* adbGetTexture(const char* filepath, SDL_Renderer* renderer);
extern SDL_Surface* adbGetSurface(const char* filepath);

#ifdef __cplusplus
}
#endif

#endif /* _adb_h */

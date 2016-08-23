#include "graphics/GraphicsDriver.h"
#include "graphics/Texture.h"
#include "graphics/TextureManager.h"
#include <SDL_image.h>
#include <stdio.h>

std::unique_ptr<TextureManager> TextureManager::sInstance;

void TextureManager::StaticInit() { sInstance.reset( new TextureManager() ); }

TextureManager::TextureManager()
{
}

TexturePtr TextureManager::GetTexture( const std::string& inTextureName )
{
    return mNameToTextureMap[inTextureName];
}
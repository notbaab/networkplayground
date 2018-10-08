#include "graphics/TextureManager.h"
#include "graphics/GraphicsDriver.h"
#include "graphics/Texture.h"

std::unique_ptr<TextureManager> TextureManager::sInstance;

void TextureManager::StaticInit() { sInstance.reset(new TextureManager()); }

TextureManager::TextureManager() {}

TexturePtr TextureManager::GetTexture(const std::string& inTextureName)
{
    return mNameToTextureMap[inTextureName];
}

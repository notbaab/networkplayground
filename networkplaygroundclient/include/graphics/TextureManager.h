#include <cstdint>
#include <graphics/Texture.h>
#include <string>
#include <unordered_map>
#include <vector>

#ifndef TextureManager_h
#define TextureManager_h

class TextureManager
{
  public:
    static void StaticInit();

    static std::unique_ptr<TextureManager> sInstance;

    TexturePtr GetTexture(const std::string& inTextureName);

  private:
    TextureManager();

    bool CachTexture(std::string inName, const char* inFileName);

    // MAKE A VECTOR!!!
    std::unordered_map<std::string, TexturePtr> mNameToTextureMap;
};
#endif /* TextureManager_h */

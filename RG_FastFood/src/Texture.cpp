#include "../include/Texture.h"
#include "../include/Util.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"


GLuint Texture::FromFile(const std::string& path) {
	return loadImageToTexture(path.c_str());
}
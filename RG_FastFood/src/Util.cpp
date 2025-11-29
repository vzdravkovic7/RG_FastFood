#include "../include/Util.h"

#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <sstream>
#include <iostream>

#include "../include/stb_image.h"

// Autor: Nedeljko Tesanovic (modifikovano radi stabilnosti)

// Sigurna funkcija za kompajliranje jednog sejdera iz fajla
unsigned int compileShader(GLenum type, const char* sourcePath)
{
    std::ifstream file(sourcePath);
    if (!file.is_open()) {
        std::cout << "Greska pri citanju fajla sa putanje \"" << sourcePath << "\"!" << std::endl;
        return 0;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    file.close();
    std::string srcStr = ss.str();
    const char* sourceCode = srcStr.c_str();

    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        if (type == GL_VERTEX_SHADER) std::cout << "VERTEX shader compile error:\n";
        else if (type == GL_FRAGMENT_SHADER) std::cout << "FRAGMENT shader compile error:\n";
        std::cout << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    std::cout << "Uspjesno procitao fajl sa putanje \"" << sourcePath << "\"!" << std::endl;
    return shader;
}

unsigned int createShader(const char* vsSource, const char* fsSource)
{
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    if (vertexShader == 0) return 0;
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return 0;
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cout << "Program link error:\n" << infoLog << std::endl;
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cout << "Program validate warning/error:\n" << infoLog << std::endl;
        // not fatal in all cases; continue
    }

    // shaders can be detached and deleted
    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth = 0;
    int TextureHeight = 0;
    int TextureChannels = 0;

    stbi_set_flip_vertically_on_load(1);
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData == nullptr) {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        return 0;
    }

    // Decide formats
    GLenum format = GL_RGB;
    GLint internal = GL_RGB8;
    if (TextureChannels == 1) { format = GL_RED; internal = GL_R8; }
    else if (TextureChannels == 3) { format = GL_RGB; internal = GL_RGB8; }
    else if (TextureChannels == 4) { format = GL_RGBA; internal = GL_RGBA8; }

    unsigned int Texture = 0;
    glGenTextures(1, &Texture);
    if (Texture == 0) {
        std::cout << "Neuspesno generisanje teksture za: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
    glBindTexture(GL_TEXTURE_2D, Texture);

    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // safety for odd row alignment
    glTexImage2D(GL_TEXTURE_2D, 0, internal, TextureWidth, TextureHeight, 0, format, GL_UNSIGNED_BYTE, ImageData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(ImageData);

    std::cout << "Ucitana tekstura: " << filePath << " (" << TextureWidth << "x" << TextureHeight << ", ch=" << TextureChannels << "). ID=" << Texture << std::endl;
    return Texture;
}

GLFWcursor* loadImageToCursor(const char* filePath) {
    int TextureWidth = 0;
    int TextureHeight = 0;
    int TextureChannels = 0;

    stbi_set_flip_vertically_on_load(0);
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 4);
    if (ImageData == nullptr) {
        std::cout << "Kursor nije ucitan! Putanja kursora: " << filePath << std::endl;
        return nullptr;
    }

    GLFWimage image;
    image.width = TextureWidth;
    image.height = TextureHeight;
    image.pixels = ImageData;

    int hotspotX = 200;
    int hotspotY = 50;

    GLFWcursor* cursor = glfwCreateCursor(&image, hotspotX, hotspotY);

    stbi_image_free(ImageData);

    if (!cursor) {
        std::cout << "glfwCreateCursor failed for " << filePath << std::endl;
    }

    return cursor;
}

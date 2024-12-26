#include "screenshot.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#include <string>
#include <algorithm>

static int pic_cnt = 0;

void captureScreen(int width, int height) {
    unsigned char *pixels = new unsigned char[width * height * 4];
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    for (int y = 0; y < height / 2; y++) {
        for (int x = 0; x < width * 4; x++) {
            std::swap(pixels[y * width * 4 + x], pixels[(height - y - 1) * width * 4 + x]);
        }
    }

    stbi_write_png(("screenshot" + std::to_string(++pic_cnt) + ".png").c_str(), width, height, 4, pixels, width * 4);
    delete[] pixels;
}
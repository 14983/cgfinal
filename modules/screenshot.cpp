#include "screenshot.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image_write.h>
#include <algorithm>

void captureScreen(int width, int height) {
    unsigned char *pixels = new unsigned char[width * height * 4];
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // 翻转图像
    for (int y = 0; y < height / 2; y++) {
        for (int x = 0; x < width * 4; x++) {
            std::swap(pixels[y * width * 4 + x], pixels[(height - y - 1) * width * 4 + x]);
        }
    }

    stbi_write_png("screenshot.png", width, height, 4, pixels, width * 4);
    delete[] pixels;
}
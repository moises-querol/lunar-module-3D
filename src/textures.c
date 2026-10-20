#include <stdio.h>
#include <stdlib.h>
#if defined(__APPLE__) && !defined(USE_FREEGLUT)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "textures.h"

enum { TEXTURE_WIDTH = 256, TEXTURE_HEIGHT = 256 };
static GLuint textureIds[TEXTURE_COUNT];

int initTextures(void) {
    glGenTextures(TEXTURE_COUNT, textureIds);
    return loadTexture("assets/textures/moon.tga", TEXTURE_GROUND) &&
           loadTexture("assets/textures/usa.tga", TEXTURE_FLAG);
}

void bindTexture(int slot) {
    if (slot >= 0 && slot < TEXTURE_COUNT)
        glBindTexture(GL_TEXTURE_2D, textureIds[slot]);
}

/* Supports uncompressed, 24-bit, bottom-left-origin TGA assets.
 * Keep the original horizontal and vertical mapping used by the model. */
int loadTexture(const char *filename, int slot) {
    unsigned char header[18];
    unsigned char *pixels = NULL;
    FILE *file = NULL;
    const size_t size = TEXTURE_WIDTH * TEXTURE_HEIGHT * 3;
    int success = 0;

    if (!filename || slot < 0 || slot >= TEXTURE_COUNT) {
        fprintf(stderr, "Invalid texture filename or slot.\n");
        return 0;
    }
    file = fopen(filename, "rb");
    if (!file) goto failure;
    if (fread(header, 1, sizeof(header), file) != sizeof(header) ||
        header[1] != 0 || header[2] != 2 || header[16] != 24 ||
        header[17] != 0 ||
        (header[12] | (header[13] << 8)) != TEXTURE_WIDTH ||
        (header[14] | (header[15] << 8)) != TEXTURE_HEIGHT ||
        fseek(file, header[0], SEEK_CUR) != 0)
        goto failure;

    pixels = malloc(size);
    if (!pixels || fread(pixels, 1, size, file) != size) goto failure;

    /* Reversing pixel order and swapping BGR to RGB is a byte reversal. */
    for (size_t i = 0; i < size / 2; ++i) {
        unsigned char value = pixels[i];
        pixels[i] = pixels[size - 1 - i];
        pixels[size - 1 - i] = value;
    }

    bindTexture(slot);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_WIDTH, TEXTURE_HEIGHT,
                 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    success = 1;

failure:
    if (!success)
        fprintf(stderr, "Could not load texture '%s': expected a complete 256x256, 24-bit, uncompressed TGA with bottom-left origin.\n", filename);
    free(pixels);
    if (file) fclose(file);
    return success;
}

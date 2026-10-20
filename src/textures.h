#ifndef TEXTURES_H
#define TEXTURES_H

enum { TEXTURE_GROUND, TEXTURE_FLAG, TEXTURE_COUNT };

/* Run after creating an OpenGL context. Failed loads preserve the old texture. */
int initTextures(void);
int loadTexture(const char *filename, int slot);
void bindTexture(int slot);

#endif

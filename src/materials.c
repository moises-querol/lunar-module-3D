#if defined(__APPLE__) && !defined(USE_FREEGLUT)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "materials.h"

static void setMaterial(const GLfloat ambient[4], const GLfloat diffuse[4],
                        const GLfloat specular[4], GLfloat shininess) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

void setGoldMaterial(void) {
    const GLfloat ambient[] = {0.28f, 0.20f, 0.06f, 1.0f};
    const GLfloat diffuse[] = {0.74f, 0.53f, 0.16f, 1.0f};
    const GLfloat specular[] = {0.85f, 0.72f, 0.38f, 1.0f};
    setMaterial(ambient, diffuse, specular, 40.0f);
}

void setAluminumMaterial(void) {
    const GLfloat ambient[] = {0.20f, 0.21f, 0.23f, 1.0f};
    const GLfloat diffuse[] = {0.62f, 0.65f, 0.70f, 1.0f};
    const GLfloat specular[] = {0.85f, 0.88f, 0.92f, 1.0f};
    setMaterial(ambient, diffuse, specular, 56.0f);
}

void setCopperMaterial(void) {
    const GLfloat ambient[] = {0.24f, 0.14f, 0.08f, 1.0f};
    const GLfloat diffuse[] = {0.65f, 0.34f, 0.18f, 1.0f};
    const GLfloat specular[] = {0.72f, 0.48f, 0.30f, 1.0f};
    setMaterial(ambient, diffuse, specular, 40.0f);
}

void setTextureMaterial(void) {
    const GLfloat ambient[] = {0.40f, 0.40f, 0.40f, 1.0f};
    const GLfloat diffuse[] = {0.90f, 0.90f, 0.90f, 1.0f};
    const GLfloat specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    setMaterial(ambient, diffuse, specular, 0.0f);
}

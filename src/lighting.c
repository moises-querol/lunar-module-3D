#if defined(__APPLE__) && !defined(USE_FREEGLUT)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "lighting.h"

void initLights(void) {
    const GLfloat ambient[] = {0.16f, 0.16f, 0.18f, 1.0f};
    const GLfloat dark[] = {0.0f, 0.0f, 0.0f, 1.0f};
    const GLfloat positions[][4] = {
        {-3.0f, 5.0f, 4.0f, 0.0f},  /* Warm key from above and left. */
        {4.0f, 2.0f, 1.0f, 0.0f},   /* Soft fill for shaded faces. */
        {0.0f, 3.0f, -4.0f, 0.0f}   /* Rim separates the far silhouette. */
    };
    const GLfloat diffuse[][4] = {
        {0.90f, 0.86f, 0.78f, 1.0f},
        {0.24f, 0.28f, 0.36f, 1.0f},
        {0.30f, 0.32f, 0.38f, 1.0f}
    };
    const GLfloat specular[][4] = {
        {0.85f, 0.82f, 0.74f, 1.0f},
        {0.10f, 0.12f, 0.16f, 1.0f},
        {0.25f, 0.28f, 0.34f, 1.0f}
    };

    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glDisable(GL_COLOR_MATERIAL);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    /* Cabin normals are authored outward independently of face winding. */
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    for (int i = 0; i < 8; ++i) glDisable(GL_LIGHT0 + i);

    /* Capture positions in eye space so rotating the model reveals its form. */
    glPushMatrix();
    glLoadIdentity();
    for (int i = 0; i < 3; ++i) {
        GLenum light = GL_LIGHT0 + i;
        glLightfv(light, GL_POSITION, positions[i]);
        glLightfv(light, GL_AMBIENT, dark);
        glLightfv(light, GL_DIFFUSE, diffuse[i]);
        glLightfv(light, GL_SPECULAR, specular[i]);
        glEnable(light);
    }
    glPopMatrix();
}

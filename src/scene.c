#include <math.h>
#include <stdlib.h>
#if defined(__APPLE__) && !defined(USE_FREEGLUT)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "math_utils.h"
#include "scene.h"
#include "materials.h"
#include "textures.h"

static float ringX(float R, int N, int n);
static float ringZ(float R, int N, int n);
static void drawCap(float R, float y,int N);
static Point3D makeVertex(float R1, float y1, int N, int n);
static Vector3D revolutionNormal(float R1, float R2, float y1, float y2, int N, int n);
static void drawStrip(float R1, float R2, float R3, float y1, float y2, float y3, int N);
static void drawSolidSurface(float H, int N, int M,float(*f)(float y));
static void drawSolidSphere(int N,int M);
static void drawSolidCylinder(int N,int M);
static void drawSolidCone(int N,int M);
static void drawPlatform(int N);
static void drawLandingLeg(int N, int withLadder);
static void drawPole(int N);
static void drawSimplePole(int N);
static void drawLunarModule(void);
static void flag(int N);
static void drawLadder(int N);
static void drawRamp(int N);
static void drawGround(void);
static void drawTerrain(void);
static void cabinShell1(void);
static void cabinShell2(void);
static void cabinShell3(void);
static void cabinShell4(void);
static void cabinShell5(void);
static void cabinShell6(void);
static void cabinShell7(void);
static void cabinShell8(void);
static void cabinShell9(void);
static void cabinShell10(void);
static void cabinShell11(void);
static void cabinTower1(void);
static void cabinTower2(void);
static void cabinTower3(void);
static void cabinTower4(void);
static void cabinTower5(void);
static void cabinTower6(void);
static void cabinTower7(void);
static void cabinTower8(void);
static void cabinTower9(void);
static void cabinTower10(void);
static void cabinTower11(void);
static void cabinTower12(void);
static void cabinTower13(void);
static void cabinTower14(void);
static void cabinTower15(void);
static void cabinDetailTop1(void);
static void cabinDetailTop2(void);
static void cabinDetailTop3(void);
static void cabinDetailTop4(void);
static void cabinDetailTop5(void);
static void cabinDetailTop6(void);
static void cabinDetailTop7(void);
static void cabinDetailTop8(void);
static void cabinDetailTop9(void);
static void cabinDetailTop10(void);
static void cabinDetailTop11(void);
static void cabinDetailTop12(void);
static void cabinDetailLower1(void);
static void cabinDetailLower2(void);
static void cabinDetailLower3(void);
static void cabinDetailLower4(void);
static void cabinDetailLower5(void);
static void cabinDetailLower6(void);
static void cabinDetailLower7(void);
static void cabinDetailLower8(void);
static void cabinDetailLower9(void);
static void cabinDetailLower10(void);
static void cabinDetailLower11(void);
static void cabinDetailLower12(void);
static void drawLunarScene(void);
static float coneRadius(float y);
static float cylinderRadius(float y);
static float sphereRadius(float y);


/*--------------------------------------------------------------------------
	Returns the x coordinate of a revolving point.
	R: revolution radius.
	N: total segments in the ring.
	n: current segment.
  --------------------------------------------------------------------------*/
static float ringX(float R, int N, int n) {
	float x = (float)R*cos(n*(2*M_PI)/N);
	return x;
}

/*--------------------------------------------------------------------------
	Returns the z coordinate of a revolving point.
	R: revolution radius.
	N: total segments in the ring.
	n: current segment.
  --------------------------------------------------------------------------*/
static float ringZ(float R, int N, int n) {
	float z = (float)R*sin(n*(2*M_PI)/N);
	return z;
}

/* Creates end caps */
static void drawCap(float R, float y,int N) {
	int i;
	float x, z;

	glBegin(GL_POLYGON);
	glNormal3f(0.0f, y < 0.0f ? -1.0f : 1.0f, 0.0f);
	for(i=0;i<=N;i++){
		int segment = y < 0.0f ? i : N - i;
		x = ringX(R, N, segment);
		z = ringZ(R, N, segment);

		glVertex3f(x,y,z);
	}	
	glEnd();
}

/*--------------------------------------------------------------------------
    Returns the 3D point for the given parameters.
    R1: first ring radius.
    y1: first ring height.
    N: total segments in the ring.
    n: current segment.
  --------------------------------------------------------------------------*/
static Point3D makeVertex(float R1, float y1, int N, int n) {

    Point3D p;

    p.x = ringX(R1, N, n);
    p.y = y1;
    p.z = ringZ(R1, N, n);

    return (p);
}
/*--------------------------------------------------------------------------
    Returns the normal of a vertex on a surface of revolution.
    R1: first ring radius.
    R2: second ring radius.
    y1: first ring height.
    y2: second ring height.
    N: total segments in the ring.
    n: current segment.
  --------------------------------------------------------------------------*/
static Vector3D revolutionNormal(float R1, float R2, float y1, float y2, int N, int n) {
    float angle = n * (2.0f * M_PI) / N;
    float height = y2 - y1;
    Vector3D normalVec = makeVector3D(height * cosf(angle), R1 - R2,
                                      height * sinf(angle));
    return normalizeVector(normalVec);
}

/* Draws strips */
static void drawStrip(float R1, float R2, float R3, float y1, float y2, float y3, int N) {
    int i;
    Point3D p;
    Vector3D n;

    glBegin(GL_QUAD_STRIP);
    for(i = 0; i <= N; i++) {
        n = revolutionNormal(R1, R2, y1, y2, N, i);
        glNormal3f(n.a, n.b, n.c);
        p = makeVertex(R1, y1, N, i);
        glVertex3f(p.x, p.y, p.z);
        /* The final ring has no following segment; reuse the last slope. */
        n = y2 == y3 ? revolutionNormal(R1, R2, y1, y2, N, i)
                     : revolutionNormal(R2, R3, y2, y3, N, i);
        glNormal3f(n.a, n.b, n.c);
        p = makeVertex(R2, y2, N, i);
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
}

/* Draws the solid */
static void drawSolidSurface(float H, int N, int M,float(*f)(float y)) {
    int i;
    float y1, y2, y3, r1, r2, r3;

    for(i = 0; i < N-1; i++) {
        y1 = i*H/(N-1)-(H/2);
        r1 = f(y1);
        y2 = (i+1)*H/(N-1)-(H/2);
        r2 = f(y2);
        if(i+2 < N) {
            y3 = (i+2)*H/(N-1)-(H/2);
            r3 = f(y3);
        }
        else {
            r3 = r2;
            y3 = y2;            
        }
        drawStrip(r1, r2, r3, y1, y2, y3, M);
    }
}

/* Draws a solid sphere */
static void drawSolidSphere(int N,int M){
	drawSolidSurface(1.0f, N, M,sphereRadius);
}

/* Draws a solid cylinder */
static void drawSolidCylinder(int N,int M){
	drawSolidSurface(1.0f,N,M,cylinderRadius);
	glColor3f(1.0f,0.0f,0.0f);
	drawCap(0.5,0.5f,M);
	drawCap(0.5,-0.5f,M);
}

/* Draws a solid cone */
static void drawSolidCone(int N,int M){
	drawSolidSurface(1.0f,N,M,coneRadius);
	drawCap(0.5f,0.5f,N);
}

/* Draws an octagonal platform with narrow beveled rims. */
static void drawPlatform(int N){
     glPushMatrix();
       glRotatef(112.5,0,1,0);
       glTranslatef(0.f,-0.04f,0.f);
       glScalef(1.75f,1.2f,1.75f);

       const float radii[] = {0.47f, 0.5f, 0.5f, 0.47f};
       const float heights[] = {-0.25f, -0.22f, 0.22f, 0.25f};
       glBegin(GL_QUADS);
       for (int band = 0; band < 3; ++band) {
         for (int side = 0; side < 8; ++side) {
           const Point3D corners[] = {
             makeVertex(radii[band], heights[band], 8, side),
             makeVertex(radii[band + 1], heights[band + 1], 8, side),
             makeVertex(radii[band + 1], heights[band + 1], 8, side + 1),
             makeVertex(radii[band], heights[band], 8, side + 1)
           };
           Vector3D normal = crossProduct(vector(corners[0], corners[1]),
                                          vector(corners[0], corners[2]));
           glNormal3f(normal.a, normal.b, normal.c);
           for (int corner = 0; corner < 4; ++corner)
             glVertex3f(corners[corner].x, corners[corner].y, corners[corner].z);
         }
       }
       glEnd();
       drawCap(radii[3], heights[3], 8);
       drawCap(radii[0], heights[0], 8);
     glPopMatrix();

     glPushMatrix();
       glTranslatef(0.f,-0.2f,0.f);
       glScalef(1.f,0.5f,1.f);
       drawSolidCylinder(N,50);
     glPopMatrix();

     glPushMatrix();
       glTranslatef(0.4f,-0.15f,0.7f);
       glScalef(0.1f,0.3f,0.1f);
       drawSolidCylinder(N,50);
     glPopMatrix();
}

/* Draws a landing leg of the lunar scene */
static void drawLandingLeg(int N, int withLadder){
     glPushMatrix();
       glRotatef(25,1,0,0);
       glTranslatef(0.f,-0.85f,0.f);
       drawPole(N);
       if (withLadder) {
         glTranslatef(-0.2f,0.f,-0.05f);
         drawLadder(N);
       }
     glPopMatrix();

     glPushMatrix();
       glRotatef(35,0,1,0);
       drawSimplePole(N);
     glPopMatrix();

     glPushMatrix();
       glRotatef(-35,0,1,0);
       drawSimplePole(N);
     glPopMatrix();

     glPushMatrix();
       glRotatef(-47.5,1,0,0);
       glTranslatef(0.f,-0.5f,0.f);
       glScalef(1.0f,0.6f,1.0f);
       drawPole(N);
     glPopMatrix();

     glPushMatrix();
       /* Add thickness upward, preserving the existing ground contact. */
       glTranslatef(0.0f,-1.48f,-0.7f);
       glScalef(0.6f,0.04f,0.6f);
       drawSolidCylinder(2,50);
     glPopMatrix();

     glPushMatrix();
       glScalef(0.2f,0.2f,0.2f);
       drawSolidSphere(N,50);
     glPopMatrix();
}

static void drawPole(int N){
     glPushMatrix();
       glScalef(0.1f,1.5f,0.1f);
       drawSolidCylinder(N,50);
     glPopMatrix();

     glPushMatrix();
       glScalef(0.1f,0.1f,0.1f);
       glRotatef(180,1,0,0);
       glTranslatef(0.0f,-8.0f,0.0f);
       drawSolidCone(N,50);
     glPopMatrix();

     glPushMatrix();
       glScalef(0.1f,0.1f,0.1f);
       glTranslatef(0.0f,-8.0f,0.0f);
       drawSolidCone(N,50);
     glPopMatrix();
}

static void drawSimplePole(int N){
     glPushMatrix();
       glRotatef(90,1,0,0);
       glTranslatef(0.0f,0.4f,0.0f);
       glScalef(1.0f,0.5f,1.0f);
       drawPole(N);
     glPopMatrix();
}

/* Draws the upper part of the lunar module. */
static void drawLunarModule(void){
     glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_POLYGON_BIT);
     glEnable(GL_POLYGON_OFFSET_FILL);
     glPolygonOffset(1.0f, 1.0f);
     glDisable(GL_CULL_FACE);
     glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
     glShadeModel(GL_FLAT);
     setAluminumMaterial();
     glPushMatrix();
      glRotatef(90,0,1,0);
      glTranslatef(0.3f,0.325f,0.8f);
      cabinTower15();
      glPushMatrix();
        glRotatef(90,0,1,0);
        glTranslatef(0.65f,0.1f,0.0f);
        cabinDetailTop12();
      glPopMatrix();

      glPushMatrix();
        glRotatef(270,0,1,0);
        glTranslatef(-0.975f,0.7f,1.0f);
        cabinDetailLower12();
      glPopMatrix();
    glPopMatrix();
     glPopAttrib();
}

static void flag(int N){
     setAluminumMaterial();
     glPushMatrix();
       glTranslatef(0.f,-0.5f,0.f);
       glScalef(0.03f,1.0f,0.03f);
       drawSolidCylinder(N,50);
     glPopMatrix();

     setTextureMaterial();
     /* Only the thin flag cloth needs illumination on both sides. */
     glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
     glEnable(GL_TEXTURE_2D);
     bindTexture(TEXTURE_FLAG);
     glBegin(GL_POLYGON);
     glNormal3f(0.0f, 0.0f, -1.0f);

     glTexCoord2f(0.0,0.0);
     glVertex2f(0.0f,0.0f);

	 glTexCoord2f(1.0,0.0);
     glVertex2f(0.5f,0.0f);

	 glTexCoord2f(1.0,1.0); 
	 glVertex2f(0.5f,-0.4f);

	 glTexCoord2f(0.0,1.0); 
	 glVertex2f(0.0f,-0.4f);

     glEnd();
     glDisable(GL_TEXTURE_2D);
     glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
}

static void drawLadder(int N){
     glPushMatrix();
       glScalef(0.05f,1.2f,0.05f);
       drawSolidCylinder(N,50);
       glTranslatef(8.0f,0.f,0.f);
       drawSolidCylinder(N,50);
     glPopMatrix();

     glPushMatrix();
        glRotatef(90,0,0,1);
        glTranslatef(0.f,-0.2f,0.f);
        glScalef(0.04f,0.4f,0.04f);
        drawSolidCylinder(N,50);
     glPopMatrix();

     glPushMatrix();
        glRotatef(90,0,0,1);
        glTranslatef(0.18f,-0.2f,0.f);
        glScalef(0.04f,0.4f,0.04f);
        drawSolidCylinder(N,50);
     glPopMatrix();

     glPushMatrix();
        glRotatef(90,0,0,1);
        glTranslatef(0.36f,-0.2f,0.f);
        glScalef(0.04f,0.4f,0.04f);
        drawSolidCylinder(N,50);
     glPopMatrix();

     glPushMatrix();
        glRotatef(90,0,0,1);
        glTranslatef(0.54f,-0.2f,0.f);
        glScalef(0.04f,0.4f,0.04f);
        drawSolidCylinder(N,50);
     glPopMatrix();

     glPushMatrix();
        glRotatef(90,0,0,1);
        glTranslatef(-0.18f,-0.2f,0.f);
        glScalef(0.04f,0.4f,0.04f);
        drawSolidCylinder(N,50);
     glPopMatrix();

     glPushMatrix();
        glRotatef(90,0,0,1);
        glTranslatef(-0.36f,-0.2f,0.f);
        glScalef(0.04f,0.4f,0.04f);
        drawSolidCylinder(N,50);
     glPopMatrix();

     glPushMatrix();
        glRotatef(90,0,0,1);
        glTranslatef(-0.54f,-0.2f,0.f);
        glScalef(0.04f,0.4f,0.04f);
        drawSolidCylinder(N,50);
     glPopMatrix();
}

static void drawRamp(int N){
     glPushMatrix();
       glBegin(GL_POLYGON);
       glNormal3f(0.0f, 1.0f, 0.0f);
       glColor3f(0.0f,0.25f,0.0f);

       glVertex3f(0.0f,0.0f,0.0f);
       glVertex3f(0.35f,0.0f,0.0f);
       glVertex3f(0.35f,0.0f,-0.8f);
       glVertex3f(0.0f,0.0f,-0.8f);

       glEnd();
     glPopMatrix();

     // Handrails
     glPushMatrix();
       glTranslatef(0.015f,0.1f,-.04f);
       glScalef(0.02f,0.2f,0.02f);
       drawSolidCylinder(N,50);
     glPopMatrix();

     glPushMatrix();
       glRotatef(90,1,0,0);
       glTranslatef(0.015f,-0.235f,-0.195f);
       glScalef(0.02f,0.4f,0.02f);
       drawSolidCylinder(N,50);
     glPopMatrix();

     glPushMatrix();
       glTranslatef(0.335f,0.1f,-.04f);
       glScalef(0.02f,0.2f,0.02f);
       drawSolidCylinder(N,50);
     glPopMatrix();

     glPushMatrix();
       glRotatef(90,1,0,0);
       glTranslatef(0.335f,-0.235f,-0.195f);
       glScalef(0.02f,0.4f,0.02f);
       drawSolidCylinder(N,50);
     glPopMatrix();
}

static void drawGround(void){
	glPushMatrix();
	 	glBegin(GL_POLYGON);
        /* The terrain transform maps local -Z to world +Y. */
        glNormal3f(0.0f, 0.0f, -1.0f);

		glTexCoord2f(0.0,0.0); 
       	glVertex2f(0.0f,0.0f);

		glTexCoord2f(0.0,1.0); 
       	glVertex2f(0.0f,1.0f);

		glTexCoord2f(1.0,1.0); 
		glVertex2f(1.0f,1.0f);

		glTexCoord2f(1.0,0.0); 
		glVertex2f(1.0f,0.0f);

       glEnd();
	glPopMatrix();
}

static void drawTerrain(void){
	glPushMatrix();
		glRotatef(90,1,0,0);
		glTranslatef(-0.5f,-0.5f,0.0f);
		drawGround();
	glPopMatrix();
}

/* Cabin faces are explicit panels, with normals derived from their triangles.
 * Draw only panel boundaries; triangulation diagonals stay invisible. */
static void drawCabinPanel(const Point3D *vertices, int count) {
    glBegin(GL_TRIANGLES);
    for (int i = 1; i + 1 < count; ++i) {
        Vector3D n = crossProduct(vector(vertices[0], vertices[i]),
                                  vector(vertices[0], vertices[i + 1]));
        if (dotProduct(n, n) == 0.0f) continue;
        glNormal3f(n.a, n.b, n.c);
        glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
        glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
        glVertex3f(vertices[i + 1].x, vertices[i + 1].y, vertices[i + 1].z);
    }
    glEnd();

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_LINE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.10f, 0.13f, 0.17f);
    glDepthFunc(GL_LEQUAL);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < count; ++i)
        glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
    glEnd();
    glPopAttrib();
}

#define DRAW_CABIN_PANEL(...) do { \
    const Point3D vertices[] = { __VA_ARGS__ }; \
    drawCabinPanel(vertices, (int)(sizeof(vertices) / sizeof(vertices[0]))); \
} while (0)


static void cabinShell1(void) {
    glPushMatrix();
        DRAW_CABIN_PANEL(
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, -0.6f},
            {0.6f, 0.0f, -0.6f},
            {0.6f, 0.0f, 0.0f}
        );
        DRAW_CABIN_PANEL(
            {0.0f, 0.0f, 0.0f},
            {0.0f, -0.2f, 0.0f},
            {0.0f, -0.2f, 0.2f}
        );
        DRAW_CABIN_PANEL(
            {0.2f, -0.2f, 0.2f},
            {0.2f, -0.2f, 0.0f},
            {0.2f, 0.0f, 0.0f}
        );
        DRAW_CABIN_PANEL(
            {0.0f, 0.0f, 0.0f},
            {0.2f, 0.0f, 0.0f},
            {0.2f, -0.2f, 0.2f},
            {0.0f, -0.2f, 0.2f}
        );
        DRAW_CABIN_PANEL(
            {0.0f, 0.0f, 0.0f},
            {0.0f, -1.4f, 0.0f},
            {0.0f, -1.4f, -0.6f},
            {0.0f, 0.0f, -0.6f}
        );

    glPopMatrix();
}

static void cabinShell2(void) {
    glPushMatrix();
        glTranslatef(-0.4f,0.2f,0.0f);
        cabinShell1();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, 0.0f, 0.0f},
        {-0.2f, 0.2f, 0.0f},
        {0.0f, 0.2f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, 0.0f, 0.0f},
        {-0.2f, 0.0f, 0.2f},
        {0.0f, 0.0f, 0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.2f, 0.0f},
        {0.2f, 0.2f, 0.0f},
        {0.0f, 0.0f, 0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.2f, 0.0f},
        {0.0f, 0.0f, 0.2f}
    );
}

static void cabinShell3(void) {
    glPushMatrix();
        glTranslatef(-0.2f,0.0f,-0.2f);
        cabinShell2();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, 0.0f, 0.0f},
        {0.0f, 0.2f, -0.2f},
        {0.0f, 0.0f, -0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, 0.0f, 0.0f},
        {-0.2f, -0.3f, 0.1f},
        {0.0f, -0.3f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.3f, 0.0f},
        {0.0f, -0.3f, -0.2f},
        {0.0f, 0.0f, -0.2f}
    );
}

static void cabinShell4(void) {
    glPushMatrix();
        glTranslatef(0.4f,0.3f,-0.1f);
        cabinShell3();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.2f, 0.0f, 0.0f},
        {0.2f, 0.3f, -0.1f},
        {0.0f, 0.3f, -0.1f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.3f, -0.1f},
        {-0.2f, 0.3f, -0.1f},
        {-0.2f, 0.0f, -0.1f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, -0.2f, 0.0f},
        {0.2f, -0.2f, 0.0f},
        {0.2f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {-0.2f, -0.2f, -0.1f},
        {0.0f, -0.2f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {-0.2f, 0.0f, -0.1f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, 0.0f, -0.1f},
        {0.0f, 0.3f, -0.1f}
    );
}

static void cabinShell5(void) {
    glPushMatrix();
        glTranslatef(-0.4f,0.2f,0.1f);
        cabinShell4();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, 0.0f, 0.1f},
        {-0.2f, 0.2f, 0.1f},
        {0.0f, 0.2f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.2f, 0.0f},
        {0.0f, 0.2f, -0.2f},
        {0.0f, 0.0f, -0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, 0.0f, 0.1f},
        {-0.2f, -0.1f, 0.15f},
        {0.0f, -0.2f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.2f, 0.0f},
        {0.0f, -0.2f, -0.2f},
        {0.0f, 0.0f, -0.2f}
    );
}

static void cabinShell6(void) {
    glPushMatrix();
        glTranslatef(0.4f,0.1f,-0.15f);
        cabinShell5();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.2f, 0.0f, 0.0f},
        {0.2f, 0.1f, -0.05f},
        {0.0f, 0.1f, -0.05f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.1f, -0.05f},
        {-0.2f, 0.1f, -0.15f},
        {-0.2f, -0.1f, -0.15f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, -0.2f, 0.05f},
        {0.2f, -0.2f, 0.05f},
        {0.2f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.2f, 0.05f},
        {-0.2f, -0.1f, -0.15f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, -0.1f, -0.15f},
        {0.0f, 0.1f, -0.05f}
    );
}

static void cabinShell7(void) {
    glPushMatrix();
        glTranslatef(0.f,0.4f,0.f);
        cabinShell6();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.2f, 0.0f, 0.0f},
        {0.2f, 0.2f, 0.05f},
        {0.0f, 0.2f, 0.05f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, 0.2f, -0.15f},
        {-0.2f, 0.3f, -0.15f},
        {0.0f, 0.2f, 0.05f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.2f, -0.15f},
        {0.2f, -0.2f, -0.15f},
        {0.2f, 0.0f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.2f, -0.15f},
        {-0.2f, 0.2f, -0.15f}
    );
}

static void cabinShell8(void) {
    glPushMatrix();
        glTranslatef(-0.4f,-0.2f,0.15f);
        cabinShell7();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.1f, 0.0f},
        {-0.2f, 0.0f, 0.2f},
        {-0.2f, 0.2f, 0.15f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.1f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {-0.2f, 0.0f, 0.2f}
    );
}

static void cabinShell9(void) {
    glPushMatrix();
        glTranslatef(0.f,0.1f,0.f);
        cabinShell8();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {-0.2f, 0.1f, 0.2f},
        {0.0f, 0.1f, 0.0f},
        {-0.2f, -0.1f, 0.15f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.1f, 0.0f},
        {-0.2f, -0.1f, 0.15f},
        {-0.2f, -0.3f, 0.0f}
    );
}

static void cabinShell10(void) {
    glPushMatrix();
        glTranslatef(0.f,0.3f,0.f);
        cabinShell9();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, 0.0f, 0.0f},
        {0.0f, 0.4f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -0.2f},
        {0.0f, 0.5f, -0.2f},
        {0.0f, 0.5f, 0.0f}
    );
}

static void cabinShell11(void) {
    glPushMatrix();
        glTranslatef(0.6f,0.f,0.f);
        cabinShell10();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.2f, 0.0f, 0.0f},
        {0.0f, 0.4f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -0.2f},
        {0.0f, 1.2f, -0.2f},
        {0.0f, 1.2f, 0.0f}
    );
}

static void cabinTower1(void) {
    glPushMatrix();
        glTranslatef(-0.6f,-1.4f,0.2f);
        cabinShell11();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -1.4f, 0.0f},
        {0.0f, -1.4f, -0.6f},
        {0.0f, 0.0f, -0.6f}
    );
}

static void cabinTower2(void) {
    glPushMatrix();
        glRotatef(180,0,1,0);
        glTranslatef(0.2f,0.2f,0.6f);
        cabinTower1();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.2f, 0.0f},
        {-0.2f, 0.2f, 0.0f},
        {0.0f, 0.0f, 0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.2f, 0.0f, 0.0f},
        {0.2f, 0.2f, 0.0f},
        {0.0f, 0.2f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.2f, 0.0f, 0.0f},
        {0.2f, 0.0f, 0.2f},
        {0.0f, 0.0f, 0.2f}
    );
}

static void cabinTower3(void) {
    glPushMatrix();
        glTranslatef(0.2f,-0.2f,0.f);
        cabinTower2();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.2f, -0.2f, 0.2f},
        {0.0f, -0.2f, 0.2f},
        {0.0f, -0.2f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, -0.2f, 0.0f},
        {0.0f, -1.4f, 0.0f},
        {0.0f, -1.4f, 0.2f},
        {0.0f, -0.2f, 0.2f}
    );
}

static void cabinTower4(void) {
    glPushMatrix();
        glTranslatef(-0.6f,0.f,0.f);
        cabinTower3();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, -0.2f, 0.0f},
        {-0.2f, 0.0f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, -0.2f, 0.2f},
        {0.0f, -0.2f, 0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, -0.2f, 0.2f},
        {-0.2f, -0.2f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -1.4f, 0.0f},
        {0.0f, -1.4f, 0.2f},
        {0.0f, -0.2f, 0.2f}
    );
}

static void cabinTower5(void) {
    glPushMatrix();
        glTranslatef(0.2f,0.5f,-0.3f);
        cabinTower4();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.3f, -0.1f},
        {-0.2f, 0.3f, -0.1f},
        {-0.2f, 0.0f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.2f, 0.3f, -0.1f},
        {0.0f, 0.3f, -0.1f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.2f, 0.3f, -0.1f},
        {0.2f, 0.0f, -0.1f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, -0.2f, 0.0f},
        {-0.2f, -0.2f, 0.0f},
        {-0.2f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.2f, -0.2f, -0.1f},
        {0.0f, -0.2f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.2f, 0.0f, -0.1f}
    );
}

static void cabinTower6(void) {
    glPushMatrix();
        glTranslatef(0.2f,0.f,0.f);
        cabinTower5();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.3f, -0.1f},
        {-0.2f, 0.3f, -0.1f},
        {-0.2f, 0.0f, -0.1f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, 0.0f, -0.1f},
        {-0.2f, -0.2f, -0.1f},
        {0.0f, -0.2f, 0.0f}
    );
}

static void cabinTower7(void) {
    glPushMatrix();
        glTranslatef(0.f,0.2f,0.f);
        cabinTower6();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.2f},
        {0.2f, 0.0f, 0.2f},
        {0.2f, 0.0f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, 0.0f, -0.1f},
        {-0.2f, -0.2f, 0.1f},
        {0.0f, 0.0f, 0.2f}
    );
}

static void cabinTower8(void) {
    glPushMatrix();
        glTranslatef(-0.2f,0.f,0.f);
        cabinTower7();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.2f, 0.0f, -0.1f},
        {0.2f, -0.2f, 0.1f},
        {0.0f, 0.0f, 0.2f}
    );
}

static void cabinTower9(void) {
    glPushMatrix();
        glTranslatef(0.f,0.f,-0.2f);
        cabinTower8();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.2f, 0.2f},
        {-0.2f, -0.2f, 0.2f},
        {-0.2f, 0.0f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.2f, -0.2f, -0.1f},
        {0.2f, -0.3f, 0.0f},
        {0.0f, -0.2f, 0.2f}
    );
}

static void cabinTower10(void) {
    glPushMatrix();
        glTranslatef(0.2f,0.f,0.f);
        cabinTower9();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, -0.2f, -0.1f},
        {-0.2f, -0.3f, 0.0f},
        {0.0f, -0.2f, 0.2f}
    );
}

static void cabinTower11(void) {
    glPushMatrix();
        glTranslatef(0.f,0.2f,-0.2f);
        cabinTower10();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.2f, 0.0f},
        {0.2f, -0.2f, 0.0f},
        {0.2f, 0.0f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, -0.1f, -0.2f},
        {-0.2f, -0.2f, -0.2f},
        {0.0f, -0.2f, 0.0f}
    );
}

static void cabinTower12(void) {
    glPushMatrix();
        glTranslatef(-0.2f,0.f,0.f);
        cabinTower11();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.2f, -0.1f, -0.2f},
        {0.2f, -0.2f, -0.2f},
        {0.0f, -0.2f, 0.0f}
    );
}

static void cabinTower13(void) {
    glPushMatrix();
        glTranslatef(0.f,0.2f,0.f);
        cabinTower12();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.3f, -0.2f},
        {-0.2f, -0.3f, -0.2f},
        {-0.2f, 0.0f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.2f, 0.0f, -0.2f},
        {0.0f, -0.3f, -0.2f}
    );
}

static void cabinTower14(void) {
    glPushMatrix();
        glTranslatef(0.4f,0.f,0.2f);
        cabinTower13();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.2f, -0.3f, 0.0f},
        {0.2f, 0.0f, 0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.4f, -0.3f},
        {0.0f, 0.1f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -0.3f},
        {0.0f, 0.4f, -0.3f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.3f, -0.3f},
        {0.0f, 0.0f, -0.3f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.3f, -0.3f},
        {0.2f, -0.3f, 0.0f}
    );
}

static void cabinTower15(void) {
    glPushMatrix();
        glTranslatef(-0.6f,0.f,0.f);
        cabinTower14();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.4f, -0.3f},
        {0.0f, 0.1f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -0.3f},
        {0.0f, 0.4f, -0.3f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.3f, -0.3f},
        {0.0f, 0.0f, -0.3f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.3f, -0.3f},
        {-0.2f, -0.3f, 0.0f}
    );
    /* Close the underside along the existing lower rim. */
    DRAW_CABIN_PANEL(
        {0.0f, -0.3f, -1.3f},
        {-0.6f, -0.3f, -1.3f},
        {-0.6f, -0.3f, -0.3f},
        {-0.4f, -0.3f, 0.0f},
        {-0.2f, -0.3f, 0.0f},
        {0.0f, -0.3f, -0.3f}
    );
}

static void cabinDetailTop1(void) {
    glPushMatrix();
        DRAW_CABIN_PANEL(
            {0.0f, 0.0f, 0.0f},
            {0.4f, 0.0f, 0.0f},
            {0.4f, 0.0f, 0.1f},
            {0.0f, 0.0f, 0.1f}
        );
        DRAW_CABIN_PANEL(
            {0.0f, 0.0f, 0.0f},
            {0.0f, -0.3f, 0.0f},
            {0.0f, -0.3f, 0.1f},
            {0.0f, 0.0f, 0.1f}
        );
    glPopMatrix();
}

static void cabinDetailTop2(void) {
    glPushMatrix();
        glTranslatef(-0.4f,0.f,-0.1f);
        cabinDetailTop1();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -0.1f},
        {0.0f, -0.3f, -0.1f},
        {0.0f, -0.3f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.4f, 0.0f, 0.0f},
        {-0.4f, -0.3f, 0.0f},
        {0.0f, -0.3f, 0.0f}
    );
}

static void cabinDetailTop3(void) {
    glPushMatrix();
        glTranslatef(0.4f,0.3f,0.f);
        cabinDetailTop2();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.1f, 0.25f},
        {0.4f, -0.1f, 0.25f},
        {0.4f, 0.0f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, -0.1f, 0.25f},
        {-0.15f, -0.2f, 0.25f},
        {0.0f, 0.0f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {-0.15f, -0.2f, 0.25f},
        {-0.15f, -0.2f, -0.1f},
        {0.0f, 0.0f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.15f, -0.2f, -0.1f},
        {0.0f, 0.0f, -0.1f}
    );
}

static void cabinDetailTop4(void) {
    glPushMatrix();
        glTranslatef(0.15f,0.2f,-0.25f);
        cabinDetailTop3();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -0.35f},
        {0.0f, -0.3f, -0.35f},
        {0.0f, -0.3f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.55f, 0.1f, 0.0f},
        {0.15f, 0.1f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.7f, 0.0f, 0.0f},
        {0.55f, 0.1f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.7f, 0.0f, 0.0f},
        {0.7f, -0.3f, 0.0f},
        {0.0f, -0.3f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.1f, 0.0f},
        {-0.15f, -0.1f, -0.05f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.15f, -0.1f, -0.05f},
        {-0.15f, -0.1f, -0.15f}
    );
    DRAW_CABIN_PANEL(
        {-0.15f, -0.1f, -0.15f},
        {0.0f, 0.0f, -0.35f},
        {0.0f, 0.0f, 0.0f}
    );
}

static void cabinDetailTop5(void) {
    glPushMatrix();
        glTranslatef(0.15f,0.1f,0.05f);
        cabinDetailTop4();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -0.1f},
        {0.0f, -0.1f, -0.1f},
        {0.0f, -0.1f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.15f, 0.0f, 0.05f},
        {0.15f, -0.1f, 0.05f},
        {0.0f, -0.1f, 0.0f}
    );
}

static void cabinDetailTop6(void) {
    glPushMatrix();
        glTranslatef(0.0f,0.1f,0.0f);
        cabinDetailTop5();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.15f, 0.0f, 0.05f},
        {0.15f, -0.1f, 0.05f},
        {0.15f, -0.1f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.15f, -0.1f, 0.0f},
        {0.15f, -0.1f, -0.1f},
        {0.0f, 0.0f, -0.1f}
    );
}

static void cabinDetailTop7(void) {
    glPushMatrix();
        glTranslatef(0.0f,0.0f,0.1f);
        cabinDetailTop6();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.15f, -0.1f, 0.0f},
        {0.15f, -0.1f, -0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.15f, -0.1f, -0.2f},
        {0.15f, 0.2f, -0.2f},
        {0.0f, 0.1f, 0.0f}
    );
}

static void cabinDetailTop8(void) {
    glPushMatrix();
        glTranslatef(-0.7f,-0.4f,0.1f);
        cabinDetailTop7();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.1f, 0.25f},
        {0.15f, -0.2f, 0.25f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.15f, -0.2f, 0.25f},
        {0.15f, -0.2f, -0.1f},
        {0.0f, 0.0f, -0.1f}
    );
}

static void cabinDetailTop9(void) {
    glPushMatrix();
        glTranslatef(-0.15f,0.5f,-0.25f);
        cabinDetailTop8();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -0.35f},
        {0.0f, 0.3f, -0.35f},
        {0.0f, 0.3f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -0.35f},
        {-0.2f, -0.1f, -0.35f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, -0.1f, -0.35f},
        {-0.5f, -0.1f, -0.35f},
        {-0.7f, 0.0f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.1f, 0.0f},
        {0.15f, 0.1f, -0.05f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.15f, 0.1f, -0.05f},
        {0.15f, 0.1f, -0.15f},
        {0.0f, 0.0f, -0.15f}
    );
}

static void cabinDetailTop10(void) {
    glPushMatrix();
        glTranslatef(-0.15f,-0.2f,0.05f);
        cabinDetailTop9();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.1f, 0.0f},
        {0.0f, -0.1f, -0.1f},
        {0.0f, 0.0f, -0.1f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.1f, 0.0f},
        {-0.15f, -0.1f, 0.05f},
        {-0.15f, 0.0f, 0.05f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.15f, 0.0f, 0.05f},
        {-0.15f, 0.1f, 0.05f},
        {-0.15f, 0.1f, -0.15f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.15f, 0.1f, -0.15f},
        {-0.15f, 0.1f, -0.3f},
        {0.0f, 0.0f, -0.1f}
    );
}

static void cabinDetailTop11(void) {
    glPushMatrix();
        glTranslatef(0.f,0.1f,0.1f);
        cabinDetailTop10();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.15f, -0.1f, 0.0f},
        {-0.15f, -0.1f, -0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.15f, -0.1f, -0.2f},
        {-0.15f, 0.2f, -0.2f},
        {0.0f, 0.1f, 0.0f}
    );
}

static void cabinDetailTop12(void) {
    glPushMatrix();
        glTranslatef(0.65f,0.2f,0.2f);
        cabinDetailTop11();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.2f, 0.1f, 0.35f},
        {-0.2f, 0.1f, 0.0f}
    );
    /* Rear closure follows the body, shoulder, and neck outlines. */
    DRAW_CABIN_PANEL(
        {-0.2f, 0.1f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.3f, 0.0f, 0.0f},
        {0.5f, 0.1f, 0.0f},
        {0.5f, 0.4f, 0.0f},
        {-0.2f, 0.4f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {-0.2f, 0.4f, 0.0f},
        {0.5f, 0.4f, 0.0f},
        {0.35f, 0.6f, 0.0f},
        {-0.05f, 0.6f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {-0.05f, 0.6f, 0.0f},
        {0.35f, 0.6f, 0.0f},
        {0.35f, 0.9f, 0.0f},
        {-0.05f, 0.9f, 0.0f}
    );
}

static void cabinDetailLower1(void) {
    glPushMatrix();
        glRotatef(90,1,0,0);
        glTranslatef(0.175f,0.25f,0.55f);
        glScalef(1.0f,.5f,1.0f);
        drawSolidCylinder(50,50);
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.7f},
        {0.35f, 0.0f, 0.7f},
        {0.35f, 0.0f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.7f},
        {0.0f, 0.0f, 0.7f}
    );
}

static void cabinDetailLower2(void) {
    glPushMatrix();
        glTranslatef(-0.35f,0.f,-0.7f);
        cabinDetailLower1();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -0.7f},
        {0.0f, -1.0f, -0.7f},
        {0.0f, -1.0f, 0.0f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.2f, 0.0f},
        {0.0f, -0.2f, 0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, -0.2f, 0.0f},
        {0.0f, -0.7f, 0.0f},
        {0.0f, -0.7f, 0.2f},
        {0.0f, -0.2f, 0.2f}
    );
}

static void cabinDetailLower3(void) {
    glPushMatrix();
        glTranslatef(0.35f,0.f,0.f);
        cabinDetailLower2();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.35f, 0.0f, 0.0f},
        {0.35f, -0.2f, 0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.35f, -0.2f, 0.2f},
        {0.0f, -0.2f, 0.2f},
        {0.0f, -0.2f, 0.0f}
    );
}

static void cabinDetailLower4(void) {
    glPushMatrix();
        glTranslatef(0.f,0.7f,0.f);
        cabinDetailLower3();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.5f, 0.0f},
        {0.0f, 0.5f, 0.2f},
        {0.0f, 0.0f, 0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.3f, 0.0f},
        {0.0f, 0.0f, 0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.35f, -0.3f, 0.0f},
        {0.35f, 0.0f, 0.0f},
        {0.35f, 0.0f, 0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, -0.3f, 0.0f},
        {0.0f, 0.0f, 0.2f},
        {0.35f, 0.0f, 0.2f},
        {0.35f, -0.3f, 0.0f}
    );
}

static void cabinDetailLower5(void) {
    glPushMatrix();
        glTranslatef(-0.35f,0.f,-0.2f);
        cabinDetailLower4();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.35f, 0.0f, 0.0f},
        {-0.35f, 0.5f, 0.0f},
        {0.0f, 0.5f, 0.0f}
    );
}

static void cabinDetailLower6(void) {
    glPushMatrix();
        glTranslatef(0.f,-0.5f,0.2f);
        cabinDetailLower5();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -0.2f},
        {0.18f, 0.0f, -0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.1f, 0.0f},
        {0.18f, -0.1f, -0.2f},
        {0.18f, 0.0f, -0.2f}
    );
}

static void cabinDetailLower7(void) {
    glPushMatrix();
        glTranslatef(0.f,0.1f,0.2f);
        cabinDetailLower6();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.18f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.2f}
    );
}

static void cabinDetailLower8(void) {
    glPushMatrix();
        glTranslatef(0.f,0.1f,0.f);
        cabinDetailLower7();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.15f, 0.1f, 0.0f},
        {0.0f, 0.1f, 0.15f}
    );
}

static void cabinDetailLower9(void) {
    glPushMatrix();
        glTranslatef(0.35f,0.f,0.f);
        cabinDetailLower8();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.15f, 0.1f, 0.0f},
        {0.0f, 0.1f, 0.15f}
    );
}

static void cabinDetailLower10(void) {
    glPushMatrix();
        glTranslatef(0.f,-0.2f,0.f);
        cabinDetailLower9();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.18f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.2f}
    );
}

static void cabinDetailLower11(void) {
    glPushMatrix();
        glTranslatef(0.f,0.f,-0.2f);
        cabinDetailLower10();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {0.0f, -0.1f, 0.0f},
        {-0.18f, -0.1f, -0.2f},
        {-0.18f, 0.0f, -0.2f}
    );
}

static void cabinDetailLower12(void) {
    glPushMatrix();
        glTranslatef(0.f,0.1f,0.f);
        cabinDetailLower11();
    glPopMatrix();

    DRAW_CABIN_PANEL(
        {0.0f, 0.0f, 0.0f},
        {-0.18f, 0.0f, -0.2f},
        {0.0f, 0.0f, -0.2f}
    );
    /* Rear and bottom caps close the attachment independently of the cabin. */
    DRAW_CABIN_PANEL(
        {0.0f, -0.7f, -0.7f},
        {0.35f, -0.7f, -0.7f},
        {0.35f, 0.3f, -0.7f},
        {0.0f, 0.3f, -0.7f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, -0.7f, -0.7f},
        {0.0f, -0.7f, 0.0f},
        {0.35f, -0.7f, 0.0f},
        {0.35f, -0.7f, -0.7f}
    );
    /* Close the rear faces of the two side fins. */
    DRAW_CABIN_PANEL(
        {0.35f, 0.0f, -0.2f},
        {0.53f, 0.0f, -0.2f},
        {0.53f, 0.1f, -0.2f},
        {0.35f, 0.1f, -0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.35f, -0.1f, -0.2f},
        {0.5f, 0.0f, -0.2f},
        {0.35f, 0.0f, -0.2f}
    );
    DRAW_CABIN_PANEL(
        {-0.18f, 0.0f, -0.2f},
        {0.0f, 0.0f, -0.2f},
        {0.0f, 0.1f, -0.2f},
        {-0.18f, 0.1f, -0.2f}
    );
    DRAW_CABIN_PANEL(
        {0.0f, -0.1f, -0.2f},
        {0.0f, 0.0f, -0.2f},
        {-0.15f, 0.0f, -0.2f}
    );
}

#undef DRAW_CABIN_PANEL

/* Terrain, flag, and lunar-module composition. */
static void drawLunarScene(void){
    setTextureMaterial();
    glPushMatrix();
        glTranslatef(0.0f,-0.75f,0.0f);
        glScalef(5.0f,1.0f,5.0f);
		glEnable(GL_TEXTURE_2D);
		bindTexture(TEXTURE_GROUND);
		drawTerrain();                                   
		glDisable(GL_TEXTURE_2D);

    glPopMatrix();

    glPushMatrix();
      glTranslatef(1.75f,0.25f,1.25f);
      flag(50);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.f,0.03f,0.f);
        setGoldMaterial(); //
        drawPlatform(20);
        setAluminumMaterial(); //
        drawLunarModule();

        const float legAngles[] = {0.0f, 180.0f, 270.0f, 90.0f};
        for (int i = 0; i < 4; ++i) {
          glPushMatrix();
            setCopperMaterial();
            glRotatef(legAngles[i], 0, 1, 0);
            glTranslatef(0.f, 0.2f, -1.2f);
            glScalef(0.65f, 0.65f, 0.65f);
            drawLandingLeg(50, i == 1);
          glPopMatrix();
        }

        glPushMatrix();
          glTranslatef(-0.175f,0.275f,1.2f);
          drawRamp(50);
        glPopMatrix();

    glPopMatrix();

}

void buildScene(void){
	/* Render solid surfaces so lighting defines the shape without a wire overlay. */
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	drawLunarScene();
}

static float coneRadius(float y) {
	return ((y+0.5f)/2);
}

static float cylinderRadius(float y) {
    (void)y;
	return(0.5);
}

static float sphereRadius(float y) {
	return((float)sqrt(fabs(0.25-(y*y))));
}

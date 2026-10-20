#ifndef MATH_H
#define MATH_H

#define EQUAL 0
#define DIFFERENT 1

typedef struct {
  float x, y, z;
} Point3D;

typedef struct {
  float a, b, c;
} Vector3D;


Vector3D crossProduct(Vector3D v, Vector3D w);
Vector3D vector(Point3D p, Point3D q);
Vector3D pointToVector(Point3D p);
float dotProduct(Vector3D v, Vector3D w);
Point3D makePoint3D(float x, float y, float z);
Vector3D makeVector3D(float a, float b, float c);
void computeTrackballMatrix(Point3D startPoint, Point3D endPoint, float matrix[], float R2);
void multiplyMatrices(float m1[], float m2[], float result[]);
int pointsDiffer(Point3D p, Point3D q);
void copyMatrix(float m1[], float m2[]);
Vector3D multiplyMatrixVector(float matrixProduct[], Vector3D v);
void rotationMatrix(float rx, float ry, float rz, float angle, float matrix[]);
Vector3D normalizeVector(Vector3D v);

#endif

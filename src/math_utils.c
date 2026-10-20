#include <math.h>
#include "math_utils.h"

/******************************************************************************/
/* Returns the position vector of a point                                 */
/* Parameters --> Point3D p: the point used to build the vector */
/* Returns --> The vector                                                   */
/******************************************************************************/
Vector3D pointToVector(Point3D p) {
  Vector3D res;

  res.a = p.x;
  res.b = p.y;
  res.c = p.z;

  return (res);
}

/******************************************************************************/
/* Builds a vector from a start point to an end point                       */
/* Parameters --> Point3D p: the start point                                 */
/*                Point3D q: the end point                                   */
/* Returns --> The vector between the points                                  */
/******************************************************************************/
Vector3D vector(Point3D p, Point3D q) {
  Vector3D res;

  res.a = q.x-p.x;
  res.b = q.y-p.y;
  res.c = q.z-p.z;
  return (res);
}

/******************************************************************************/
/* Updates the coordinates of a point                                      */
/* Parameters --> float x: the new x coordinate                    */
/*                float y: same for y                                     */
/*                float z: same for z                                     */
/* Returns --> The updated point                                  */
/******************************************************************************/
Point3D makePoint3D(float x, float y, float z) {
  Point3D p;

  p.x = x;
  p.y = y;
  p.z = z;

  return (p);
}

/******************************************************************************/
/* Updates the components of a vector                                     */
/* Parameters --> float a: the new a component                    */
/*                float b: same for b                                     */
/*                float c: same for c                                     */
/* Returns --> The updated vector                                 */
/******************************************************************************/
Vector3D makeVector3D(float a, float b, float c) {
  Vector3D v;

  v.a = a;
  v.b = b;
  v.c = c;

  return (v);
}

/******************************************************************************/
/* Dot product of two vectors                                           */
/* Parameters --> Vector3D v: one vector                             */
/*                Vector3D w: the other vector                                  */
/* Returns --> The dot product value                               */
/******************************************************************************/
float dotProduct(Vector3D v, Vector3D w) {
  float result = 0.0f;

  result = (v.a*w.a)+(v.b*w.b)+(v.c*w.c);

  return (result);
}

/******************************************************************************/
/* Cross product of two vectors                                         */
/* Parameters --> Vector3D v: one vector                             */
/*                Vector3D w: the other vector                                  */
/* Returns --> The resulting vector                                        */
/******************************************************************************/
Vector3D crossProduct(Vector3D v, Vector3D w) {
  float length;
  Vector3D res;
  
  res.a = v.b*w.c-(v.c*w.b);
  res.b = v.c*w.a-(v.a*w.c);
  res.c = v.a*w.b-(v.b*w.a);
  length = (float)sqrt((res.a*res.a)+(res.b*res.b)+(res.c*res.c));
  if(length != 0) {
    res.a /= length;
    res.b /= length;
    res.c /= length;
  }

  return (res);
}

/******************************************************************************/
/* Normalizes a vector                                                        */
/* Parameters --> Vector3D v: the vector to normalize                */
/* Returns --> The normalized vector                                       */    
/******************************************************************************/
Vector3D normalizeVector(Vector3D v) {
  float length = (float)sqrt(dotProduct(v, v));

  if(length != 0) {
    v.a /= length;
    v.b /= length;
    v.c /= length;
  }

  return (v);
}

/******************************************************************************/
/* Checks whether two points differ                                      */
/* Parameters --> Point3D p: one point                                */
/*                Point3D q: the other                                          */
/* Returns --> DIFFERENT o EQUAL                                         */    
/******************************************************************************/
int pointsDiffer(Point3D p, Point3D q) {
  int pointsDiffer = EQUAL;

  if(p.x != q.x || p.y != q.y || p.z != q.z) pointsDiffer = DIFFERENT;

  return pointsDiffer;
}

/******************************************************************************/
/* Computes the model-view matrix from two grab points and the       */
/* virtual trackball radius                                                 */
/* Parameters --> Point3D startPoint: initial grab point.                   */
/*                Point3D endPoint: final grab point.                        */
/*                matrix[]: the computed matrix                          */
/*                float R2: square of the virtual trackball radius        */
/* Returns --> None                                                     */
/******************************************************************************/
void computeTrackballMatrix(Point3D startPoint, Point3D endPoint,
                            float matrix[], float R2) {
    Vector3D startVector, endVector, normal;
    float cosine, angle;
    if (R2 <= 0.0f || !pointsDiffer(startPoint, endPoint)) {
        rotationMatrix(0.0f, 1.0f, 0.0f, 1.0f, matrix);
        return;
    }
    startVector = normalizeVector(makeVector3D(startPoint.x, startPoint.y,
        sqrtf(fmaxf(0.0f, R2 - startPoint.x * startPoint.x - startPoint.y * startPoint.y))));
    endVector = normalizeVector(makeVector3D(endPoint.x, endPoint.y,
        sqrtf(fmaxf(0.0f, R2 - endPoint.x * endPoint.x - endPoint.y * endPoint.y))));
    normal = crossProduct(endVector, startVector);
    if (dotProduct(normal, normal) == 0.0f) {
        rotationMatrix(0.0f, 1.0f, 0.0f, 1.0f, matrix);
        return;
    }
    cosine = fmaxf(-1.0f, fminf(1.0f, dotProduct(startVector, endVector)));
    angle = acosf(cosine) * 5.0f;
    if (sinf(angle) < 0.0f) {
        normal.a = -normal.a;
        normal.b = -normal.b;
        normal.c = -normal.c;
    }
    rotationMatrix(normal.a, normal.b, normal.c, cosf(angle), matrix);
}

/******************************************************************************/
/* Computes a rotation matrix around an arbitrary axis                 */
/* Parameters --> float rx, ry, rz: rotation axis                        */
/*                float cosine: rotation cosine                         */
/*                float matrix[]: output rotation matrix     */
/* Returns --> None                                                     */
/******************************************************************************/
void rotationMatrix(float rx, float ry, float rz, float cosine, float matrix[]) {
  cosine = fmaxf(-1.0f, fminf(1.0f, cosine));
  float sine = sqrtf(fmaxf(0.0f, 1.0f - cosine * cosine));
  float oneMinusCos = 1.0f - cosine;

  matrix[0] = (rx*rx*oneMinusCos) + cosine;
  matrix[1] = (rx*ry*oneMinusCos) + (rz*sine);
  matrix[2] = (rx*rz*oneMinusCos) - (ry*sine);
  matrix[4] = (rx*ry*oneMinusCos) - (rz*sine);
  matrix[5] = (ry*ry*oneMinusCos) + (cosine);
  matrix[6] = (ry*rz*oneMinusCos) + (rx*sine);
  matrix[8] = (rx*rz*oneMinusCos) + (ry*sine);
  matrix[9] = (ry*rz*oneMinusCos) - (rx*sine);
  matrix[10] = (rz*rz*oneMinusCos) + (cosine);
  matrix[3] = matrix[7] = matrix[11] = 0.0f;
  matrix[12] = matrix[13] = 0.0f; matrix[14] = 0.0f;
  matrix[15] = 1.0f;
}

/******************************************************************************/
/* Multiplies two 4x4 matrices                                                */
/* Parameters --> float m1[]: one matrix.                            */
/*                float m2[]: the other.                                        */
/*                float result[]=m1[]*m2[].                                */
/******************************************************************************/
void multiplyMatrices(float m1[], float m2[], float result[]) {
  float length;

  result[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2] + m1[12]*m2[3];
  result[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7];
  result[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11];
  result[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15];
  result[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2] + m1[13]*m2[3];
  result[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6] + m1[13]*m2[7];
  result[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10] + m1[13]*m2[11];
  result[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13]*m2[15];
  result[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
  result[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
  result[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
  result[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
  result[3] = 0.0f; result[7] = 0.0f; result[11] = 0.0f;
  result[15] = 1.0f;

  length = (float)sqrt(result[0]*result[0] + result[1]*result[1] + result[2]*result[2]);
  result[0] /= length;
  result[1] /= length;
  result[2] /= length;

  length = (float)sqrt(result[4]*result[4] + result[5]*result[5] + result[6]*result[6]);
  result[4] /= length;
  result[5] /= length;
  result[6] /= length;

  length = (float)sqrt(result[8]*result[8] + result[9]*result[9] + result[10]*result[10]);
  result[8] /= length;
  result[9] /= length;
  result[10] /= length;
}

/******************************************************************************/
/* Copies one matrix into another.                                               */
/* Parameters --> float m1[]: destination matrix.                                 */
/*                float m2[]: source matrix.                                  */
/* Returns --> None                                                    */
/******************************************************************************/
void copyMatrix(float m1[], float m2[]) {
  int i;

  for(i = 0; i < 16; i++) m1[i] = m2[i];
}

/******************************************************************************/
/* Multiplies a matrix by a vector.                                       */
/* Parameters --> float matrixProduct[]: matrix multiplier                  */
/*                float Vector3D v: vector to multiply             */
/* Returns --> Vector3D: multiplication result                 */
/******************************************************************************/
Vector3D multiplyMatrixVector(float matrixProduct[], Vector3D v) {
  Vector3D result;

  result.a = matrixProduct[0] * v.a + matrixProduct[4] * v.b + matrixProduct[8] * v.c;
  result.b = matrixProduct[1] * v.a + matrixProduct[5] * v.b + matrixProduct[9] * v.c;
  result.c = matrixProduct[2] * v.a + matrixProduct[6] * v.b + matrixProduct[10] * v.c;

  return (result);
}

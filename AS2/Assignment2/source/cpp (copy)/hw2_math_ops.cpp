#include <cstdio>
#include <cmath>
#include <stdlib.h>
#include "hw2_math_ops.h"
#define ABS(a) ((a)>0?(a):-1*(a))

/*
 * Calculate cross product of vec3 a, vec3 b and return resulting vec3.
 */
Vec3 crossProductVec3(Vec3 a, Vec3 b) {
    Vec3 result;

    result.x = a.y * b.z - b.y * a.z;
    result.y = b.x * a.z - a.x * b.z;
    result.z = a.x * b.y - b.x * a.y;

    return result;
}

/*
 * Calculate dot product of vec3 a, vec3 b and return resulting value.
 */
double dotProductVec3(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/*
 * Find length (|v|) of vec3 v.
 */
double lengthOfVec3(Vec3 v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

/*
 * Normalize the vec3 to make it unit vec3.
 */
Vec3 normalizeVec3(Vec3 v) {
    Vec3 result;
    double d;

    d = lengthOfVec3(v);
    result.x = v.x / d;
    result.y = v.y / d;
    result.z = v.z / d;

    return result;
}

/*
 * Add vec3 a to vec3 b and return resulting vec3 (a+b).
 */
Vec3 addVec3(Vec3 a, Vec3 b) {
    Vec3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;

    return result;
}

/*
 * Subtract vec3 b from vec3 a and return resulting vec3 (a-b).
 */
Vec3 subtractVec3(Vec3 a, Vec3 b) {
    Vec3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;

    return result;
}

/*
 * Multiply each element of vec3 with scalar.
 */
Vec3 multiplyVec3WithScalar(Vec3 v, double c) {
    Vec3 result;
    result.x = v.x * c;
    result.y = v.y * c;
    result.z = v.z * c;

    return result;
}

/*
 * Prints elements in a vec3. Can be used for debugging purposes.
 */
void printVec3(Vec3 v) {
    printf("(%2.2lf,%2.2lf,%2.2lf)\n", v.x, v.y, v.z);
}

/*
 * Check whether vec3 a and vec3 b are equal.
 * In case of equality, returns 1.
 * Otherwise, returns 0.
 */
int areEqualVec3(Vec3 a, Vec3 b) {
    double e = 0.000000001;

    /* if x difference, y difference and z difference is smaller than threshold, then they are equal */
    if ((ABS((a.x - b.x)) < e) && (ABS((a.y - b.y)) < e) && (ABS((a.z - b.z)) < e)) {
        return 1;
    } else {
        return 0;
    }
}


/*
 * Makes matrix m (double[4][4]) an identity matrix (values on the diagonal are 1, others are 0).
*/
void makeIdentityMatrix(double m[4][4]) {
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (i == j) m[i][j] = 1.0;
            else m[i][j] = 0.0;
}

/*
 * Multiply matrices m1 (double[4][4]) and m2 (double[4][4]) and store the result in result matrix r (double[4][4]).
 */
void multiplyMatrixWithMatrix(double r[4][4], double m1[4][4], double m2[4][4]) {
    int i, j, k;
    double total;

    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++) {
            total = 0;
            for (k = 0; k < 4; k++)
                total += m1[i][k] * m2[k][j];
            r[i][j] = total;
        }
}

/*
 * Multiply matrix m (double[4][4]) with vector v (double[4]) and store the result in vector r (double[4]).
 */
void multiplyMatrixWithVec4d(double r[4], double m[4][4], double v[4]) {
    int i, j;
    double total;
    for (i = 0; i < 4; i++) {
        total = 0;
        for (j = 0; j < 4; j++)
            total += m[i][j] * v[j];
        r[i] = total;
    }
}

/*
 * Multiply matrix m (double[3][3]) with vector v (double[3]) and store the result in vector r (double[3]).
 */
void multiplyMatrixWithVec3d(double r[3], double m[3][3], double v[3]){
    int i, j;
    double total;
    for (i = 0; i < 3; i++) {
        total = 0;
        for (j = 0; j < 3; j++)
            total += m[i][j] * v[j];
        r[i] = total;
    }
}


/*
* Get Slope of line between two points: a, b . Ignores the Z coordinate
*/
double get2DSlope(Vec3 a, Vec3 b){
     return (a.y-b.y)/(a.x-b.x);
}

/*
* swaps vectors, becomes handy when dealing with line drawing
*/
void swap(Vec3 a, Vec3 b){
     double tmp;
     tmp = a.x;
     a.x = b.x;
     b.x = tmp;

     tmp = a.y;
     a.y = b.y;
     b.y = tmp;

     tmp = a.z;
     a.z = b.z;
     b.z = tmp;

}


/*
* subtracts colors, becomes handy when dealing with line  coloring
*/
Color subtractColors(Color a, Color b){
     Color r;
     int tmp;
     tmp = round(a.r-b.r);
     if(tmp>0)
          r.r = tmp;
     else
          r.r = 0;
          // printf("%s\n","ERROR NEG COLOR");
     tmp = round(a.g-b.g);
     if(tmp>0)
          r.g = tmp;
     else
          r.g = 0;
          // printf("%s\n","ERROR NEG COLOR");

     tmp = round(a.b-b.b);
     if(tmp>0)
          r.b = tmp;
     else
          r.b = 0;
          // printf("%s\n","ERROR NEG COLOR");
     return r;
}

/*
* adds colors, becomes handy when dealing with line  coloring
*/
Color addColors(Color a, Color b){
     Color r;
     r.r = round(a.r+b.r);
     r.g = round(a.g+b.g);
     r.b = round(a.b+b.b);
     return r;
}

/*
* prints a 4x4 matrix
*/
void printM4(double m[4][4]){
printf("\n");
     for(int i=0;i<4;i++){
          for (int j = 0; j < 4; j++) {
               printf("%2.2lf  ",m[i][j]);
          }
          printf("\n");
     }
printf("\n");
}



/*
* divides a color by number, becomes handy when dealing with line  coloring
*/
Color divideColorByVal(Color a , double val){
     Color r;
     if(val > 0.000001){
          r.r = (int) round(a.r/val);
          r.g = (int) round(a.g/val);
          r.b = (int) round(a.b/val);
          return r;
     } else {
          r.r =0;
          r.g =0;
          r.b =0;
          return r;
     }
}

/*
* computes and alpha,beta,gamma in the results matrix
*/
void getBarycentric(double x, double y,Vec3 a, Vec3 b, Vec3 c , double results[3]){

     Vec3 p;
     p.x=x;
     p.y=y;
     p.z=0;
     Vec3 v0 = subtractVec3(a , b);
     Vec3 v1 = subtractVec3(c , b);
     Vec3 v2 = subtractVec3(p , b);
     // Vec3 v0 = subtractVec3(b , a);
     // Vec3 v1 = subtractVec3(c , a);
     // Vec3 v2 = subtractVec3(p , a);
     v0.z=0;
     v1.z=0;
     v2.z=0;

     float d00 = dotProductVec3(v0, v0);
     float d01 = dotProductVec3(v0, v1);
     float d11 = dotProductVec3(v1, v1);
     float d20 = dotProductVec3(v2, v0);
     float d21 = dotProductVec3(v2, v1);
     float denom = d00 * d11 - d01 * d01;
     double alpha = (d11 * d20 - d01 * d21) / denom;
     double beta  = (d00 * d21 - d01 * d20) / denom;
     double gamma = 1.0f - alpha - beta;
     results[0]=alpha;
     results[2]=beta;
     results[1]=gamma;

}


/*
* returns a copy of a vector
*/
Vec3 copyOfVec3(Vec3 v){
     Vec3 copy;
     copy.x=v.x;
     copy.y=v.y;
     copy.z=v.z;
     copy.colorId=v.colorId;
     return copy;
}

/*
* copies the content of the source to the distination
*/
void copyM4toM4(double dist[4][4], double source[4][4]){
     for(int i=0;i<4;i++)
          for(int j=0;j<4;j++)
               dist[i][j]=source[i][j];
}


/*
* Performs Translation t on vector v
*/
void translate(Vec3* v, Translation t){
     double m[4][4]= {{1 , 0 , 0 , t.tx},
               {0 , 1 , 0 , t.ty},
               {0 , 0 , 1 , t.tz},
               {0 , 0 , 0 , 1   }};
    double R[4];
    double V[4];
    V[0]=(*v).x;V[1]=(*v).y;V[2]=(*v).z;V[3]=1;

    // printVec3(*v);
    multiplyMatrixWithVec4d(R,m,V);
    // printf("HERE >>> %.1lf %.1lf %.1lf\n",R[0],R[1],R[2] );
    (*v).x=R[0];(*v).y=R[1];(*v).z=R[2];
    // printVec3(*v);
    // exit(1);
}

void translate_MATRIX(Translation t, double M[4][4]){
     double m[4][4]= {{1 , 0 , 0 , t.tx},
                    {0 , 1 , 0 , t.ty},
                    {0 , 0 , 1 , t.tz},
                    {0 , 0 , 0 , 1   }};

     copyM4toM4(M,m);

}

/*
* Performs Scaling s on vector v
*/
void scale(Vec3 *v, Scaling s){
     double m[4][4]= {{s.sx , 0 , 0 , 0},
                    {0 , s.sy , 0 , 0},
                    {0 , 0 , s.sz , 0},
                    {0 , 0 , 0 ,    1}};
     double R[4];
     double V[4];
     V[0]=(*v).x;V[1]=(*v).y;V[2]=(*v).z;V[3]=1;
     multiplyMatrixWithVec4d(R,m,V);
     // printf("HERE >>> %.1lf %.1lf %.1lf\n",R[0],R[1],R[2] );
     (*v).x=R[0];(*v).y=R[1];(*v).z=R[2];
}

void scale_MATRIX(Scaling s, double M[4][4]){
     double m[4][4]= {{s.sx , 0 , 0 , 0},
                    {0 , s.sy , 0 , 0},
                    {0 , 0 , s.sz , 0},
                    {0 , 0 , 0 ,    1}};
     copyM4toM4(M,m);
}

/*
* Performs Rotation r on vector v
*/
// void rotate(Vec3 *v, Rotation r){
//      Vec3 u;
//      u.x=r.ux; u.y=r.uy; u.z=r.uz;
//      u=normalizeVec3(u);
//
//      double a,b,c,d;
//      a=u.x;b=u.y;c=u.z;
//      d = sqrt(a*a+b*b);
//      //rotate alpha
//      double ra[4][4]= {{1 ,   0   ,    0   ,     0},
//                       {0 , (c/d) , (-b/d) ,     0},
//                       {0 , (b/d) , (c/d)  ,     0},
//                       {0 ,   0   ,   0    ,    1 }};
//
//      //rotate beta
//      double rb[4][4]= {{sqrt(a*a+b*b)/sqrt(a*a+b*b+c*c) ,   0   ,    -a/sqrt(a*a+b*b+c*c)            , 0},
//                       {              0                 ,   1   ,           0                        , 0},
//                       {a/sqrt(a*a+b*b+c*c)             ,   0   ,     sqrt(a*a+b*b)/sqrt(a*a+b*b+c*c), 0},
//                       {0                               ,   0   ,                 0               ,    1 }};
//
//      //rotate around z
//      double angleRad=(r.angle/180)*M_PI;
//
//      double rz[4][4]= {{cos(angleRad) ,   -sin(angleRad)   ,    0   ,     0},
//                       {sin(angleRad) ,  cos(angleRad)     ,   0    ,     0},
//                       {0             ,     0                , 1  ,     0},
//                       {0             ,       0            ,   0    ,    1 }};
//
//      //rotate -beta
//      double rnb[4][4]= {{sqrt(a*a+b*b)/sqrt(a*a+b*b+c*c) ,   0   ,    a/sqrt(a*a+b*b+c*c)            , 0},
//                    {              0                 ,   1   ,           0                        , 0},
//                    {-a/sqrt(a*a+b*b+c*c)             ,   0   ,     sqrt(a*a+b*b)/sqrt(a*a+b*b+c*c), 0},
//                    {0                               ,   0   ,                 0               ,    1 }};
//      //rotate -alpha
//      double rna[4][4]= {{1 ,   0   ,    0   ,     0},
//                       {0 , (c/d) , (b/d)  ,     0},
//                       {0 , (-b/d) , (c/d) ,     0},
//                       {0 ,   0   ,   0    ,    1 }};
//
//
//      double R[4];
//      double V[4];
//      V[0]=(*v).x;V[1]=(*v).y;V[2]=(*v).z;V[3]=1;
//      multiplyMatrixWithVec4d(R,ra,V);
//      V[0]=R[0];V[1]=R[1];V[2]=R[2];
//      multiplyMatrixWithVec4d(R,rb,V);
//      V[0]=R[0];V[1]=R[1];V[2]=R[2];
//      multiplyMatrixWithVec4d(R,rz,V);
//      V[0]=R[0];V[1]=R[1];V[2]=R[2];
//      multiplyMatrixWithVec4d(R,rnb,V);
//      V[0]=R[0];V[1]=R[1];V[2]=R[2];
//      multiplyMatrixWithVec4d(R,rna,V);
//      V[0]=R[0];V[1]=R[1];V[2]=R[2];
//
//      (*v).x=R[0];(*v).y=R[1];(*v).z=R[2];
//
// }


void rotate2(Vec3 *v, Rotation r){
     Vec3 u;
     u.x=r.ux; u.y=r.uy; u.z=r.uz;
     u=normalizeVec3(u);

     Vec3 vv;
     vv.x=0.53; vv.y=33.8; vv.z=296.835;
     vv = normalizeVec3(vv);
     Vec3 w;
     vv = crossProductVec3(u,vv);
     vv = normalizeVec3(vv);
     w = crossProductVec3(u,vv);
     w = normalizeVec3(w);

     double m[4][4] = {{ u.x , u.y , u.z , 0 },
                       { vv.x , vv.y , vv.z , 0 },
                       { w.x , w.y , w.z , 0 },
                       {  0  ,  0  ,  0  , 1 },};

     double m_inverse[4][4] = {{ u.x , vv.x , w.x , 0 },
                              { u.y , vv.y , w.y , 0 },
                              { u.z , vv.z , w.z , 0 },
                              {  0  ,  0  ,  0  , 1 },};

     double angleRad=(r.angle/180.0)*M_PI;
     double rx[4][4]= {{ 1         ,         0          ,    0   ,     0},
                    {0             ,      cos(angleRad) ,   -sin(angleRad) ,     0},
                    {0             ,      sin(angleRad) ,  cos(angleRad)     ,   0 },
                    {0             ,       0            ,   0    ,    1 }};


     double tmp[4][4];
     double Result[4][4];
     double R[4];
     multiplyMatrixWithMatrix(tmp,m_inverse,rx);
     multiplyMatrixWithMatrix(Result,tmp,m);
     double V[4];
     V[0]=(*v).x;V[1]=(*v).y;V[2]=(*v).z;V[3]=1;
     multiplyMatrixWithVec4d(R,Result,V);
     (*v).x=R[0];(*v).y=R[1];(*v).z=R[2];

}


void rotate_MATRIX(Rotation r, double M[4][4]){

     Vec3 u;
     u.x=r.ux; u.y=r.uy; u.z=r.uz;
     u=normalizeVec3(u);

     Vec3 vv;
     vv.x=0.53; vv.y=33.8; vv.z=296.835;
     vv = normalizeVec3(vv);
     Vec3 w;
     vv = crossProductVec3(u,vv);
     vv = normalizeVec3(vv);
     w = crossProductVec3(u,vv);
     w = normalizeVec3(w);

     double m[4][4] = {{ u.x , u.y , u.z , 0 },
                       { vv.x , vv.y , vv.z , 0 },
                       { w.x , w.y , w.z , 0 },
                       {  0  ,  0  ,  0  , 1 },};

     double m_inverse[4][4] = {{ u.x , vv.x , w.x , 0 },
                              { u.y , vv.y , w.y , 0 },
                              { u.z , vv.z , w.z , 0 },
                              {  0  ,  0  ,  0  , 1 },};

     double angleRad=(r.angle/180.0)*M_PI;
     // double angleRad=(r.angle);
     double rx[4][4]= {{ 1         ,         0          ,    0   ,     0},
                    {0             ,      cos(angleRad) ,   -sin(angleRad) ,     0},
                    {0             ,      sin(angleRad) ,  cos(angleRad)     ,   0 },
                    {0             ,       0            ,   0    ,    1 }};


     double tmp[4][4];
     double Result[4][4];
     double R[4];
     multiplyMatrixWithMatrix(tmp,m_inverse,rx);
     multiplyMatrixWithMatrix(Result,tmp,m);

     copyM4toM4(M,Result);
}

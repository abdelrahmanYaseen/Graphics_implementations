/*
     Group Members :
          Abdelrahman Yaseen 2175578
          Khaled Eldowa 2269256

*/

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <limits>
#include "hw2_types.h"
#include "hw2_math_ops.h"
#include "hw2_file_ops.h"
#include <iostream>
using namespace std;
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

double inf = std::numeric_limits<double>::infinity();
Camera cameras[100];
int numberOfCameras = 0;

Model models[1000];
int numberOfModels = 0;

Color colors[100000];
int numberOfColors = 0;

Translation translations[1000];
int numberOfTranslations = 0;

Rotation rotations[1000];
int numberOfRotations = 0;

Scaling scalings[1000];
int numberOfScalings = 0;

Vec3 vertices[100000];
int numberOfVertices = 0;

Color backgroundColor;

// backface culling setting, default disabled
int backfaceCullingSetting = 0;

Color **image;


/*
* Get Slope of line between two points: a, b . Ignores the Z coordinate
*/
double get2DSlope(Vec3 a, Vec3 b){
     return double(a.y-b.y)/double(a.x-b.x);
}

/*
* swaps vectors, becomes handy when dealing with line drawing
*/
void swap(Vec3 *a, Vec3 *b){
     //
     double tmp;
     Vec3 temp = copyOfVec3(*a);
     Vec3 temp2 = copyOfVec3(*b);

     (*a).x = temp2.x;
     (*a).y = temp2.y;
     (*a).z = temp2.z;
     (*a).colorId = temp2.colorId;

     (*b).x = temp.x;
     (*b).y = temp.y;
     (*b).z = temp.z;
     (*b).colorId = temp.colorId;

}


/*
* subtracts colors, becomes handy when dealing with line  coloring
*/
Color subtractColors(Color a, Color b){
     Color r;
     r.r = a.r-b.r;
     r.g = a.g-b.g;
     r.b = a.b-b.b;
     return r;
}

/*
* adds colors, becomes handy when dealing with line  coloring
*/
Color addColors(Color a, Color b){
     Color r;
     r.r = a.r+b.r;
     r.g = a.g+b.g;
     r.b = a.b+b.b;
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
     r.r = a.r/val;
     r.g = a.g/val;
     r.b = a.b/val;
     return r;
}
Color multiplyColorByVal(Color a , double val){
     Color r;
     r.r = a.r*val;
     r.g = a.g*val;
     r.b = a.b*val;
     return r;
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
     results[1]=beta;
     results[2]=gamma;

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



void translate_MATRIX(Translation t, double M[4][4]){
     double m[4][4]= {{1 , 0 , 0 , t.tx},
                    {0 , 1 , 0 , t.ty},
                    {0 , 0 , 1 , t.tz},
                    {0 , 0 , 0 , 1   }};

     copyM4toM4(M,m);

}


void scale_MATRIX(Scaling s, double M[4][4]){
     double m[4][4]= {{s.sx , 0 , 0 , 0},
                    {0 , s.sy , 0 , 0},
                    {0 , 0 , s.sz , 0},
                    {0 , 0 , 0 ,    1}};
     copyM4toM4(M,m);
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



/*
	Initializes image with background color
*/
void initializeImage(Camera cam) {
    int i, j;

    for (i = 0; i < cam.sizeX; i++)
        for (j = 0; j < cam.sizeY; j++) {
            image[i][j].r = backgroundColor.r;
            image[i][j].g = backgroundColor.g;
            image[i][j].b = backgroundColor.b;

        }
}


/*
     Transform all the vertices of the scene to have coordinates in a
     local coordinates system where the camera is at the origin
*/

void cameraTransform_MATRIX(Camera cam, double m[4][4]){

     // printf("->>> %lf %lf %lf\n",cam.pos.x,cam.pos.y,cam.pos.z );
     // printf("-> %lf %lf %lf\n",cam.v.x,cam.v.y,cam.v.z );
     // printf("-> %lf %lf %lf\n",cam.w.x,cam.w.y,cam.w.z );

     double M1[4][4] = {{1 , 0 , 0 , -cam.pos.x},
                           {0 , 1 , 0 , -cam.pos.y},
                           {0 , 0 , 1 , -cam.pos.z},
                           {0 , 0 , 0 , 1   }};

     double M2[4][4] = {{cam.u.x , cam.u.y , cam.u.z , 0},
                        {cam.v.x , cam.v.y , cam.v.z , 0},
                        {cam.w.x , cam.w.y , cam.w.z , 0},
                        {0       ,       0 ,     0   , 1}};


     // double M3[4][4] = {{cam.u.x,cam.u.y,cam.u.z,-(cam.u.x*cam.pos.x + cam.u.y*cam.pos.y + cam.u.z*cam.pos.z)},
     //                       {cam.v.x,cam.v.y,cam.v.z,-(cam.v.x*cam.pos.x + cam.v.y*cam.pos.y + cam.v.z*cam.pos.z)},
     //                       {cam.w.x,cam.w.y,cam.w.z,-(cam.w.x*cam.pos.x + cam.w.y*cam.pos.y + cam.w.z*cam.pos.z)},
     //                       {    0  ,    0  ,   0    ,                        1                            }};

     multiplyMatrixWithMatrix(m,M2,M1);
     // copyM4toM4(m,matrix);

}
/*
     Performing the orthogonal and perspective transformation
     on all the vertices of the scene
*/

void projectionTransform_MATRIX(Camera cam, double m[4][4]){
     double matrix[4][4] = {{(2*cam.n)/(cam.r-cam.l),             0.0         ,(cam.r+cam.l)/(cam.r-cam.l),0.0},
                       {      0.0                ,(2*cam.n)/(cam.t-cam.b),(cam.t+cam.b)/(cam.t-cam.b),0.0},
                       {      0.0                ,             0.0         ,-(cam.f+cam.n)/(cam.f-cam.n),-(2*cam.f*cam.n)/(cam.f-cam.n)},
                       {      0.0                ,             0.0         ,   -1    ,                        0.0                            }};


     copyM4toM4(m,matrix);
}

/*
     maps the vertices to the screen
*/
void viewportTransform_MATRIX(Camera cam, double m[4][4]){
     double matrix[4][4] = {{cam.sizeX/2.0,     0.0      ,      0.0       ,(cam.sizeX-1)/2.0},
                       {   0.0       , cam.sizeY/2.0,      0.0       ,(cam.sizeY-1)/2.0 },
                       {   0.0       ,    0.0       ,      0.5     ,    0.5         },
                       {   0.0       ,    0.0       ,      0.0       ,     1          }};

     copyM4toM4(m,matrix);
}

void printverticies(){
     printf("%s\n","--------------------------------------------------------" );
     for(int c = 1; c <= numberOfVertices;c++){

          printf("%5d-> [%2.1lf, %2.1lf, %2.1lf]\n",c,vertices[c].x,vertices[c].y,vertices[c].z );
     }
     printf("%s\n","--------------------------------------------------------" );
}

void applyMatrixToVerticies(double m[4][4]){

     double tmp[4], r[4];
     for(int k =1;k<=numberOfVertices;k++){
         tmp[0]=vertices[k].x;
         tmp[1]=vertices[k].y;
         tmp[2]=vertices[k].z;
         tmp[3]=1;
         multiplyMatrixWithVec4d(r,m,tmp);
         // printf("%s\n",">" );
         // printVec3(vertices[i]);
         vertices[k].x=r[0];
         vertices[k].y=r[1];
         vertices[k].z=r[2];
         // printVec3(vertices[i]);
     }
}

int inList(int item, int list[100000], int limit){
     for(int i=0;i<limit;i++){
          if(list[i]==item)
               return 1;
     }
     return 0;
}
void printArray(int list[100000], int limit){
     cout<<"PRINTING LIST : "<<limit<<endl;
     // for(int i=0;i<limit;i++){
     //      printf("%d  ",list[i] );
     // }
     cout<<endl;
}
void modelingTransform_MATRIX(double modellingTransMatrix[4][4], Model model){
     // double modellingTransMatrix[4][4];
     // makeIdentityMatrix(modellingTransMatrix);
     double tmp[4][4];
     double modellingTransMatrix_copy[4][4];

	for (int j = 0; j < model.numberOfTransformations; j++) {
          int transformationID=model.transformationIDs[j];
          makeIdentityMatrix(tmp);
          switch(model.transformationTypes[j])
          {
          	case 't':
               translate_MATRIX(translations[transformationID],tmp);
          		break;
          	case 's':
               scale_MATRIX(scalings[transformationID],tmp);
          		break;
          	case 'r':
               rotate_MATRIX(rotations[transformationID],tmp);

          		break;
          }
          copyM4toM4(modellingTransMatrix_copy,modellingTransMatrix);
          multiplyMatrixWithMatrix(modellingTransMatrix,tmp,modellingTransMatrix_copy);
    }
}

int maxi(int a, int b, int c){
     if(a>=b)
          if(a>=c)
          return a;
     if(b>=a)
          if(b>=c)
               return b;
     return c;
}
int mini(int a, int b, int c){
     if(a<=b)
          if(a<=c)
               return a;
     if(b<=a)
          if(b<=c)
               return b;
     return c;
}
Vec3 tuneVec3(Camera cam,Vec3 v){
     Vec3 r;

	if (v.x > cam.sizeX-1) r.x= cam.sizeX-1;
     else r.x= v.x; //round(v.x);

     if (v.y > cam.sizeY-1) r.y= cam.sizeY-1;
     else r.y= v.y;//round(v.y);

     // if (v.z > 1.0)
     //      v.z=1;
     // if (v.z < -1.0)
     //      v.z=-1;

     if(r.x < 0)
          r.x=0;
     if(r.y < 0)
          r.y=0;

     r.colorId=v.colorId;
     return r;
}

void fill(Camera cam, Vec3 a, Vec3 b, Vec3 c){
     Color col;
     int xMax = maxi((int)round(a.x),(int)round(b.x),(int)round(c.x));
     int xMin = mini((int)round(a.x),(int)round(b.x),(int)round(c.x));
     int yMin = mini((int)round(a.y),(int)round(b.y),(int)round(c.y));
     int yMax = maxi((int)round(a.y),(int)round(b.y),(int)round(c.y));



     double results[3],alpha,beta,gamma;
     for(int x =xMin; x <= xMax; x++){
          for(int y=yMin; y<=yMax; y++){
               col.r=col.g=col.b=0;
               getBarycentric(x,y,a,b,c,results);
               // alpha=results[0];
               // beta=results[1];
               // gamma=results[2];
               alpha=results[0];
               beta=results[2];
               gamma=results[1];
               if(alpha>=0 && beta>=0 && gamma>=0){
                    col = addColors(col,multiplyColorByVal(colors[a.colorId],(alpha)));
                    col = addColors(col,multiplyColorByVal(colors[b.colorId],(beta)));
                    col = addColors(col,multiplyColorByVal(colors[c.colorId],(gamma)));
                    image[x][y]=col;
               }
          }
     }
}
void drawLine(Camera cam,Vec3 a, Vec3 b){
     Vec3 A,B;
     A.x=a.x;A.y=a.y;A.z=a.z;A.colorId=a.colorId;
     B.x=b.x;B.y=b.y;B.z=b.z;B.colorId=b.colorId;
     if(A.x<0)
          A.x=0;
     if(B.x<0)
          B.x=0;
     if(A.x>cam.sizeX-1)
          A.x=cam.sizeX-1;
     if(B.x>cam.sizeX-1)
          B.x=cam.sizeX-1;
     if(A.y<0)
          A.y=0;
     if(B.y<0)
          B.y=0;
     if(A.y>cam.sizeY-1)
          A.y=cam.sizeY-1;
     if(B.y>cam.sizeY-1)
          B.y=cam.sizeY-1;

    if( A.y > B.y){
         swap(&A,&B);
    } else if (A.y == B.y){
         if(A.x > B.x)
               swap(&A,&B);
    }

     double m = get2DSlope(A,B);

    int dx, dy, d, x,y;
    dx = (B.x - A.x);
    dy = (A.y - B.y);
    x = A.x, y = A.y;
    Color c0 = colors[A.colorId];
    Color c1 = colors[B.colorId];

    Color dc = subtractColors(c1,c0);
     dc = divideColorByVal(dc,lengthOfVec3(subtractVec3(B,A)));
    Color c = c0;

     if (A.x == B.x) {
          while (y <= B.y){
            image[x][y]=c;
       	  y++;
            c = addColors(dc,c);
         }
    } else if (A.y == B.y) {
          x = MIN(A.x,B.x);
          while (x <= MAX(A.x,B.x)){
            image[x][int(A.y)]=c;
       	  x++;
            c = addColors(dc,c);
         }
     } else if (m >= 0 && m < 1){
      d = 2 * dy + dx;
      while (x <= B.x)	{
       image[x][y]=c;
    	  x++;
    	  if (d < 0){
    	      d +=  2*dy + 2*dx;
    	      y++;
    	  } else {
    	    d +=  2*dy;
    	   }
       c = addColors(dc,c);
      }
    } else if (m >= 1 && m < inf){
      d =  dy + 2*(dx);

      while (y <= B.y){
       image[x][y]=c;
    	  if (d < 0){
    	   d += 2*dx;
    	  }else{
    	       x++;
    	    d +=  2*dy + 2*dx;
    	  }
    	  y++;
       c = addColors(dc,c);
      }
    } else if (m >= -inf && m < -1){
      d =  -dy + 2*(dx);
      while (y <= B.y){
       image[x][y]=c;
    	  if (d < 0){
	       x--;
	       d +=  -2*dy + 2*dx;
    	  }else{

    	       d += 2*dx;
    	  }
    	  y++;
       c = addColors(dc,c);
      }
    }else if (m >= -1 && m < 0){
      d =  -2*dy + (dx);
      while (x >= B.x){
       image[x][y]=c;
    	  if (d < 0){
            d += -2*dy;
    	  } else {
           d +=  -2*dy + 2*dx;
           y++;
    	  }
    	  x--;
       c = addColors(dc,c);
      }
    }
}

/*
	Transformations, culling, rasterization are done here.
	You can define helper functions inside this file (rasterizer.cpp) only.
	Using types in "hw2_types.h" and functions in "hw2_math_ops.cpp" will speed you up while working.
*/
void forwardRenderingPipeline(Camera cam) {
     double MATRIX[4][4];
     double MATRIX_VPT[4][4];
     double holder1[4][4];
     double holder2[4][4];
     double vertex[4];
     double result[4];
     int numberOfTransformedVerticies=0;

     for(int i =0; i < numberOfModels; i++){
          int transformedVertecies[100000];
          makeIdentityMatrix(MATRIX);
          makeIdentityMatrix(holder1);
          makeIdentityMatrix(holder2);

          modelingTransform_MATRIX(holder1,models[i]);
          copyM4toM4(MATRIX,holder1);


          cameraTransform_MATRIX(cam,holder1);
          multiplyMatrixWithMatrix(holder2,holder1,MATRIX);
          copyM4toM4(MATRIX,holder2);

          projectionTransform_MATRIX(cam,holder1);
          multiplyMatrixWithMatrix(holder2,holder1,MATRIX);
          copyM4toM4(MATRIX,holder2);

          viewportTransform_MATRIX(cam,MATRIX_VPT);



          numberOfTransformedVerticies=0;
          for(int k =0; k <models[i].numberOfTriangles; k++){
               Vec3 normal,v;
               Vec3 vector1,vector2;
               Vec3 *a = &vertices[models[i].triangles[k].vertexIds[0]];
               Vec3 *b = &vertices[models[i].triangles[k].vertexIds[1]];
               Vec3 *c = &vertices[models[i].triangles[k].vertexIds[2]];
               int flag_a=0,flag_b=0,flag_c=0;
               if(inList(models[i].triangles[k].vertexIds[0],transformedVertecies,numberOfTransformedVerticies)){
                    flag_a=1;
               } else {
                    transformedVertecies[numberOfTransformedVerticies++]=models[i].triangles[k].vertexIds[0];
               }
               if(inList(models[i].triangles[k].vertexIds[1],transformedVertecies,numberOfTransformedVerticies)){
                    flag_b=1;
               } else {
                    transformedVertecies[numberOfTransformedVerticies]=models[i].triangles[k].vertexIds[1];
                    numberOfTransformedVerticies++;
               }
               if(inList(models[i].triangles[k].vertexIds[2],transformedVertecies,numberOfTransformedVerticies)){
                    flag_c=1;
               } else {
                    transformedVertecies[numberOfTransformedVerticies]=models[i].triangles[k].vertexIds[2];
                    numberOfTransformedVerticies++;
               }

               if(!flag_a){
                    vertex[0] = (*a).x;
                    vertex[1] = (*a).y;
                    vertex[2] = (*a).z;
                    vertex[3] = 1;
                    multiplyMatrixWithVec4d(result,MATRIX,vertex);
                    (*a).x = result[0]/(result[3]);
                    (*a).y = result[1]/(result[3]);
                    (*a).z = result[2]/(result[3]);
               }

               if(! flag_b){
                    vertex[0] = (*b).x;
                    vertex[1] = (*b).y;
                    vertex[2] = (*b).z;
                    vertex[3] = 1;
                    multiplyMatrixWithVec4d(result,MATRIX,vertex);
                    (*b).x = result[0]/(result[3]);
                    (*b).y = result[1]/(result[3]);
                    (*b).z = result[2]/(result[3]);
               }
               if(! flag_c){
                    vertex[0] = (*c).x;
                    vertex[1] = (*c).y;
                    vertex[2] = (*c).z;
                    vertex[3] = 1;
                    multiplyMatrixWithVec4d(result,MATRIX,vertex);
                    (*c).x = result[0]/(result[3]);
                    (*c).y = result[1]/(result[3]);
                    (*c).z = result[2]/(result[3]);
               }

               if(! flag_a){
                    vertex[0] = (*a).x;
                    vertex[1] = (*a).y;
                    vertex[2] = (*a).z;
                    vertex[3] = 1;
                    multiplyMatrixWithVec4d(result,MATRIX_VPT,vertex);
                    (*a).x = result[0];
                    (*a).y = result[1];
                    (*a).z = result[2];
               }

               if(! flag_b){

                    vertex[0] = (*b).x;
                    vertex[1] = (*b).y;
                    vertex[2] = (*b).z;
                    vertex[3] = 1;
                    multiplyMatrixWithVec4d(result,MATRIX_VPT,vertex);
                    (*b).x = result[0];
                    (*b).y = result[1];
                    (*b).z = result[2];
               }
               if(! flag_c){

                    vertex[0] = (*c).x;
                    vertex[1] = (*c).y;
                    vertex[2] = (*c).z;
                    vertex[3] = 1;
                    multiplyMatrixWithVec4d(result,MATRIX_VPT,vertex);
                    (*c).x = result[0];
                    (*c).y = result[1];
                    (*c).z = result[2];
               }

               double val;

               vector1 = subtractVec3(*a,*b);
               vector1 = normalizeVec3(vector1);
               vector2 = subtractVec3(*c,*b);
               vector2 = normalizeVec3(vector2);

               v = subtractVec3(*b,cam.pos);
               v = normalizeVec3(v);
               normal=crossProductVec3(vector1,vector2);
			normal=normalizeVec3(normal);
			val=dotProductVec3(normal,v);

               if( backfaceCullingSetting && val < 0.000000001)
                    continue;

               if(models[i].type == 1)
                    fill(cam,tuneVec3(cam,*c),tuneVec3(cam,*a),tuneVec3(cam,*b));
               else {
                    drawLine(cam,tuneVec3(cam,*a),tuneVec3(cam,*b));
                    drawLine(cam,tuneVec3(cam,*b),tuneVec3(cam,*c));
                    drawLine(cam,tuneVec3(cam,*c),tuneVec3(cam,*a));
               }


          }
          // exit(1);
     }
}




int main(int argc, char **argv){
    int i, j;

    if (argc < 2) {
        std::cout << "Usage: ./rasterizer <scene file> <camera file>" << std::endl;
        return 1;
    }
    // printf("%s\n","Helloooww");
    // read camera and scene files
    readSceneFile(argv[1]);
    readCameraFile(argv[2]);

    image = 0;

    Vec3 copyOfVerticies[100000];
    for(i=1;i<=numberOfVertices;i++){
         copyOfVerticies[i]=copyOfVec3(vertices[i]);
    }


    for (i = 0; i < numberOfCameras; i++) {
    // for (i = 0; i < 1; i++) {

        // allocate memory for image
        if (image) {
			for (j = 0; j < cameras[i].sizeX; j++) {
		        delete image[j];
		    }

			delete[] image;
		}

        image = new Color*[cameras[i].sizeX];

        if (image == NULL) {
            std::cout << "ERROR: Cannot allocate memory for image." << std::endl;
            exit(1);
        }

        for (j = 0; j < cameras[i].sizeX; j++) {
            image[j] = new Color[cameras[i].sizeY];
            if (image[j] == NULL) {
                std::cout << "ERROR: Cannot allocate memory for image." << std::endl;
                exit(1);
            }
        }


        // initialize image with basic values

        initializeImage(cameras[i]);


     forwardRenderingPipeline(cameras[i]);


/*
 0 0 - 800 500
 0 0 - 500 800
 500 0 - 0 200 **
 500 0 - 400 800
500 0 - 500 500**
200 200 - 400 200 * one direction works, the other doesn't;
###############
*/
     // Vec3 a;
     // Vec3 b;
     // Vec3 c;
     // Color r;
     // r.r=250;r.g=0;r.b=0;
     // Color rr;
     // rr.r=0;rr.g=250;rr.b=0;
     //
     // c.x=120;c.y=400;c.z=0;c.colorId=4;
     // b.x=200;b.y=200;b.z=0;b.colorId=3;
     // a.x=20;a.y=200;a.z=0;a.colorId=2;
     // // swap(&a,&c);
     // // 4 :Blue
     // // 3 : Green
     // // 2 : Red
     // printVec3(a);
     // printVec3(b);
     // printVec3(c);
     //
     // printf("%s\n","___________\n" );
     // // drawLine(cameras[0],a,b);
     // fill(cameras[i],a,b,c);
     //
     // for(int xx = a.x - 10 ; xx < a.x+10; xx++)
     //      for(int yy = a.y - 10 ; yy < a.y+10; yy++)
     //           image[xx][yy]=colors[a.colorId];
     // for(int xx = b.x - 10 ; xx < b.x+10; xx++)
     //      for(int yy = b.y - 10 ; yy < b.y+10; yy++)
     //           image[xx][yy]=colors[b.colorId];
     // for(int xx = c.x - 10 ; xx < c.x+10; xx++)
     //      for(int yy = c.y - 10 ; yy < c.y+10; yy++)
     //           image[xx][yy]=colors[c.colorId];

     // exit(1);
     // printverticies();


        //do the image

        // exit(1);
        // prepareImage(cameras[i],1,1);

        // generate PPM file
        writeImageToPPMFile(cameras[i]);

        // Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
        // Notice that os_type is not given as 1 (Ubuntu) or 2 (Windows), below call doesn't do conversion.
        // Change os_type to 1 or 2, after being sure that you have ImageMagick installed.

        // convertPPMToPNG(cameras[i].outputFileName, 99);

        for(int c=1;c<=numberOfVertices;c++){
             vertices[c]=copyOfVec3(copyOfVerticies[c]);
        }
        // printverticies();
    }
    printf("%s\n","FINISHED SUCCESSFULLY");
    return 0;

}




/*
// C++ program for Mid-point line generation
#include<bits/stdc++.h>
using namespace std;

// Header file for including graphics functions
// #include<graphics.h>

// midPoint function for line generation
void midPoint (int X0, int Y0, int X1, int Y1){
  double m = (float(Y1) - Y0)/ (float(X1) - X0);

  // calculate dx & dy
    int dx, dy, d, x,y;
    dx = X1 - X0;
    dy = Y0 - Y1;
    x = X0, y = Y0;
  if (m >= 0 && m < 1){
      cout<<"1st "<< m <<endl;
      d = 2 * dy + dx;

      while (x <= X1)	{
    	  cout << x << "," << y << "\n";
    	  x++;
    	  if (d < 0){
    	      d +=  2*dy + 2*dx;
    	      y++;
    	  } else {
    	    d +=  2*dy;
    	    }
      }
    } else if (m >= 1 && m < 100000){
      cout<<"2nd"<<endl;
      d =  dy + 2*(dx);

      while (y <= Y1){
    	  cout << x << "," << y << "\n";
    	  if (d < 0){
    	   d += 2*dx;
    	  }else{
    	       x++;
    	    d +=  2*dy + 2*dx;
    	  }
    	  y++;
      }
    } else if (m >= -100000000 && m < -1){
      cout<<"3nd"<<endl;
      d =  -dy + 2*(dx);

      while (y <= Y1){
    	  cout << x << "," << y << "\n";
    	  if (d < 0){
	       x--;
	       d +=  -2*dy + 2*dx;
    	  }else{

    	       d += 2*dx;
    	  }
    	  y++;
      }
    }else if (m >= -1 && m < 0){
      cout<<"4th"<<endl;
      d =  -2*dy + (dx);
      while (x >= X1){
    	  cout << x << "," << y << " d:"<< d << "\n";
    	  if (d < 0){

    	   d +=  -2*dy - 2*dx;
    	   y++;

    	  }else {
    	   d += 2*dy;
    	  }
    	  x--;
      }
    } else {
      cout << "HEHE";
    }
}


// Driver program
int
main ()
{
  // graphics driver and mode
  // used in graphics.h
  // int gd = DETECT, gm;

  // Initialize graphics function
  // initgraph (&gd, &gm, "");

  int X1 = 0, Y1 = 0, X2 = -100, Y2 = 5;
  midPoint (X1, Y1, X2, Y2);
  return 0;
  // double alpha=(1.0*(b.y-c.y)*(x-b.x)+(c.x-b.x)*(y-b.y))/((b.y-c.y)*(x0-b.x)+(c.x-b.x)*(a.y-b.y));
  // double beta=(1.0*(c.y-a.y)*(x-c.x)+(a.x-c.x)*(y-c.y))/((c.y-a.y)*(b.x-c.x)+(a.x-c.x)*(b.y-c.y));
  // double gamma=(1.0*(a.y-b.y)*(x-a.x)+(b.x-a.x)*(y-a.y))/((a.y-b.y)*(c.x-a.x)+(b.x-a.x)*(c.y-a.y));

}


*/

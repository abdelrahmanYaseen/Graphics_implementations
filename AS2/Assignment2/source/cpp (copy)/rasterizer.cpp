// WARNING
// THe code currently deals with one camera only
// the pipeline mofifies the vertices matrix itself
// other cameras can't deal withe the matrix since it was modified
// need to copy the matrix in the future

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
     if(a>b)
          if(a>c)
          return a;
     if(b>a)
          if(b>c)
               return b;
     return c;
}
int mini(int a, int b, int c){
     if(a<b)
          if(a<c)
          return a;
     if(b<a)
          if(b<c)
               return b;
     return c;
}
Vec3 convertIndices(Camera cam,Vec3 v){
     Vec3 r;

	if (v.x > cam.sizeX) r.x= cam.sizeX-1;
     else r.x= v.x; //round(v.x);

     if (v.y > cam.sizeY) r.y= cam.sizeY-1;
     else r.y= v.y;//round(v.y);

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
     // cout<<"Hi"<<xMax<<" "<< xMin<<endl;
     double results[3],alpha,beta,gamma;
     for(int x =xMin; x <= xMax; x++){
          for(int y=yMin; y<=yMax; y++){
               col.r=col.g=col.b=0;
               getBarycentric(x,y,a,b,c,results);
               alpha=results[0];
               beta=results[1];
               gamma=results[2];
               // alpha=results[2];
               // beta=results[0];
               // gamma=results[1];
               if(alpha==0)
                    alpha=0.000001;
               if(beta==0)
                    beta=0.000001;
               if(gamma==0)
                    gamma=0.000001;
               if(alpha>=0 && beta>=0 && gamma>=0){
                    col = addColors(col,divideColorByVal(colors[a.colorId],(1.0/alpha)));
                    col = addColors(col,divideColorByVal(colors[b.colorId],(1.0/beta)));
                    col = addColors(col,divideColorByVal(colors[c.colorId],(1.0/gamma)));
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
     if(A.x>cam.sizeX)
          A.x=cam.sizeX-1;
     if(B.x>cam.sizeX)
          B.x=cam.sizeX-1;
     if(A.y<0)
          A.y=0;
     if(B.y<0)
          B.y=0;
     if(A.y>cam.sizeY)
          A.y=cam.sizeY-1;
     if(B.y>cam.sizeY)
          B.y=cam.sizeY-1;

    double m = get2DSlope(a,b);
    // printf("%s\n","HRER" );
    if( a.y > b.y)
          swap(a,b);
    int dx, dy, d, x,y;
    dx = B.x - a.x;
    dy = A.y - B.y;
    x = A.x, y = A.y;
    Color c0 = colors[A.colorId];
    Color c1 = colors[B.colorId];
    // printf("%s\n","HRER" );
    Color dc = subtractColors(c1,c0);
    dc = divideColorByVal(dc,B.x-A.x);
    Color c = c0;
  if (m >= 0 && m < 1){
      // cout<<"1st "<< m <<endl;
      d = 2 * dy + dx;
      while (x <= B.x)	{
    	  // cout << x << "," << y << "\n";
       // cout << c.r <<" " << c.g << " " << c.b<<endl;
       image[x][y]=c;
       // printf("%s\n","HRE1111R" );
    	  x++;
    	  if (d < 0){
    	      d +=  2*dy + 2*dx;
    	      y++;
    	  } else {
    	    d +=  2*dy;
    	   }
       c = addColors(dc,c);
       // printf("%s\n","HRasdasdER" );
      }
    } else if (m >= 1 && m < inf){
      // cout<<"2nd"<<endl;
      d =  dy + 2*(dx);

      while (y <= B.y){
    	  // cout << x << "," << y << "\n";
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
      // cout<<"3nd"<<endl;
      d =  -dy + 2*(dx);

      while (y <= B.y){
    	  // cout << x << "," << y << "\n";
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
      // cout<<"4th"<<endl;
      d =  -2*dy + (dx);
      while (x >= B.x){
    	  // cout << x << "," << y << " d:"<< d << "\n";
       image[x][y]=c;
    	  if (d < 0){

    	   d +=  -2*dy - 2*dx;
    	   y++;

    	  }else {
    	   d += 2*dy;
    	  }
    	  x--;
       c = addColors(dc,c);
      }
    } else {
         //Vertical
         // cout<<"Vertical"<<endl;
         while (y <= B.y){
       	  // cout << x << "," << y << " d:"<< d << "\n";
            image[x][y]=c;
       	  y++;
            c = addColors(dc,c);
         }
    }
}

/*
	Transformations, culling, rasterization are done here.
	You can define helper functions inside this file (rasterizer.cpp) only.
	Using types in "hw2_types.h" and functions in "hw2_math_ops.cpp" will speed you up while working.
*/
void forwardRenderingPipeline(Camera cam, int culling) {
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
               // printArray(transformedVertecies,numberOfTransformedVerticies);
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
                    (*a).x = result[0]/result[3];
                    (*a).y = result[1]/result[3];
                    (*a).z = result[2]/result[3];
               }

               if(! flag_b){
                    vertex[0] = (*b).x;
                    vertex[1] = (*b).y;
                    vertex[2] = (*b).z;
                    vertex[3] = 1;
                    multiplyMatrixWithVec4d(result,MATRIX,vertex);
                    (*b).x = result[0]/result[3];
                    (*b).y = result[1]/result[3];
                    (*b).z = result[2]/result[3];
               }
               if(! flag_c){
                    vertex[0] = (*c).x;
                    vertex[1] = (*c).y;
                    vertex[2] = (*c).z;
                    vertex[3] = 1;
                    multiplyMatrixWithVec4d(result,MATRIX,vertex);
                    (*c).x = result[0]/result[3];
                    (*c).y = result[1]/result[3];
                    (*c).z = result[2]/result[3];
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
               vector2= subtractVec3(*c,*b);
               vector2= normalizeVec3(vector2);

               v = subtractVec3(*b,cam.pos);
               v = normalizeVec3(v);
               normal=crossProductVec3(vector1,vector2);
			normal=normalizeVec3(normal);
			val=dotProductVec3(normal,v);
               if( culling && val < 0.0000000001)
                    continue;

               fill(cam,convertIndices(cam,*c),convertIndices(cam,*a),convertIndices(cam,*b));
               // drawLine(cam,convertIndices(cam,*a),convertIndices(cam,*b));
               // drawLine(cam,convertIndices(cam,*c),convertIndices(cam,*b));
               // drawLine(cam,convertIndices(cam,*c),convertIndices(cam,*a));


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


     forwardRenderingPipeline(cameras[i],1);
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
}


*/

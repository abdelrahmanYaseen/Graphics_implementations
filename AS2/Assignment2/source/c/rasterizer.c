#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <math.h>
#define ABS(a) ((a)>0?(a):-1*(a))

typedef struct {
    double x, y, z, w;
    int colorId;
} Vec4;

typedef struct {
    Vec4 vertices4[3];
} Triangle4;
typedef struct {
    double x, y, z;
    int colorId;
} Vec3;

typedef struct {
    double tx, ty, tz;
} Translation;

typedef struct {
    double angle, ux, uy, uz;
} Rotation;

typedef struct {
    double sx, sy, sz;
} Scaling;

typedef struct {
    int cameraId;
    Vec3 pos;
    Vec3 gaze;
    Vec3 v;
    Vec3 u;
    Vec3 w;
    double l, r, b, t;
    double n;
    double f;
    int sizeX;
    int sizeY;
    char outputFileName[80];
} Camera;

typedef struct {
    double r, g, b;
} Color;

typedef struct {
    int vertexIds[3];
} Triangle;

typedef struct {
    int modelId;
    int type;
    int numberOfTransformations;
    int transformationIDs[1000];
    char transformationTypes[1000];
    int numberOfTriangles;
    Triangle triangles[25000];
} Model;

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

void fillInTriangle(int x0,int y0,Color c0,int x1,int y1,Color c1,int x2,int y2,Color c2);
void midPointAlgorithm(int x0,int y0,Color c0,int x1,int y1,Color c1);
int convertXIndices (double d,Camera cam);
int convertYIndices (double d,Camera cam);
void writeImage(Camera camera);
void performTranslation(double Matrix[4][4], double tx, double ty, double tz) ;
void performScaling(double Matrix[4][4], double sx, double sy, double sz) ;
void performRotation(double Matrix[4][4],double InverseMatrix[4][4],double Rotation[4][4], double angle1, Vec3 u) ;
void rotateCamera(double Matrix[4][4], Vec3 u, Vec3 v, Vec3 w) ;
void copyMatrix(double M1[4][4],double M2[4][4]) ;
void forwardRenderingPipeline(Camera cam);
void readCameraFile(char *camFileName) ;
void readSceneFile(char *sceneFileName) ;
int make_between_0_255(double value) ;
void writeImageToPPMFile(Camera camera) ;
void convertPPMToPNG(char *ppmFileName, int os_type);
Vec3 crossProductVec3(Vec3 a, Vec3 b);
double dotProductVec3(Vec3 a, Vec3 b);
double lengthOfVec3(Vec3 v);
Vec3 normalizeVec3(Vec3 v);
Vec3 addVec3(Vec3 a, Vec3 b) ;
Vec3 subtractVec3(Vec3 a, Vec3 b);
Vec3 multiplyVec3WithScalar(Vec3 v, double c);
void printVec3(Vec3 v) ;
int areEqualVec3(Vec3 a, Vec3 b) ;
void makeIdentityMatrix(double m[4][4]);
void multiplyMatrixWithMatrix(double r[4][4], double m1[4][4], double m2[4][4]) ;
void multiplyMatrixWithVec4d(double r[4], double m[4][4], double v[4]) ;
void initializeImage(Camera cam);
int main(int argc, char **argv) {
    int i, j;

    if (argc < 2) {
        printf("Usage: ./rasterizer <scene file> <camera file>\n");
        return 1;
    }

    // read camera and scene files
    readSceneFile(argv[1]);
    readCameraFile(argv[2]);

    image = 0;

   // for (i = 0; i < numberOfCameras; i++)
   for (i = 0; i < numberOfCameras; i++){

        // allocate memory for image
        if (image) free(image);

        image = (Color **) malloc(sizeof(Color *) * (cameras[i].sizeX));

        if (image == NULL) {
            printf("ERROR: Cannot allocate memory for image.");
            exit(1);
        }

        for (j = 0; j < cameras[i].sizeX; j++) {
            image[j] = (Color *) malloc(sizeof(Color) * (cameras[i].sizeY));
            if (image[j] == NULL) {
                printf("ERROR: Cannot allocate memory for image.");
                exit(1);
            }
        }


        // initialize image with basic values
        initializeImage(cameras[i]);

        // do forward rendering pipeline operations
        forwardRenderingPipeline(cameras[i]);
	/*	Color c0,c1;
       c0.r=0;
       c0.g=0;
       c0.b=0;
       c1.r=100;
       c1.g=100;
       c1.b=100;
		midPointAlgorithm(1,6,c0,8,4,c1);
		for (j=10-1;j>=0;j--)
	{
		for (i=0;i<10;i++)
		{
			printf("%d ",convert(image[i][j].r));

		}
		printf("\n");
	}
	*/
		writeImage(cameras[i]);
        // generate PPM file
    //    writeImageToPPMFile(cameras[i]);

        // Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
        // Notice that os_type is not given as 1 (Ubuntu) or 2 (Windows), below call doesn't do conversion.
        // Change os_type to 1 or 2, after being sure that you have ImageMagick installed.
     //   convertPPMToPNG(cameras[i].outputFileName, 99);
    }

    return 0;

}



void performTranslation(double Matrix[4][4], double tx, double ty, double tz) {
    Matrix[0][0]=1.0;
	Matrix[0][1]=0.0;
	Matrix[0][2]=0.0;
	Matrix[0][3]=tx;
	Matrix[1][0]=0.0;
	Matrix[1][1]=1.0;
	Matrix[1][2]=0.0;
	Matrix[1][3]=ty;
	Matrix[2][0]=0.0;
	Matrix[2][1]=0.0;
	Matrix[2][2]=1.0;
	Matrix[2][3]=tz;
	Matrix[3][0]=0.0;
	Matrix[3][1]=0.0;
	Matrix[3][2]=0.0;
	Matrix[3][3]=1.0;
}

void performScaling(double Matrix[4][4], double sx, double sy, double sz)
{
	int k,z;
	for(k=0;k<4;k++)
	{
		for(z=0;z<4;z++)
		{
			if((z==k)&&(z==0))
			{
				Matrix[k][z]=sx;
			}
			else if((z==k)&&(z==1))
			{
				Matrix[k][z]=sy;
			}
			else if((z==k)&&(z==2))
			{
				Matrix[k][z]=sz;
			}
			else if((z==k)&&(z==3))
			{
				Matrix[k][z]=1.0;
			}
			else{
				Matrix[k][z]=0.0;
			}
		}
	}
}

void performRotation(double Matrix[4][4],double InverseMatrix[4][4],double Rotation[4][4], double angle1, Vec3 u)
{
	double minValue;
    char minCoordinate;
    Vec3 v,w;
    double angle=(angle1/180.0)*M_PI;

	minCoordinate='x';
	minValue=u.x;
	if(u.y<minValue)
	{
		minCoordinate='y';
		minValue=u.y;
	}
	if(u.z<minValue)
	{
		minCoordinate='z';
		minValue=u.z;
	}

	if(minCoordinate=='x')
	{
		v.x=0.0;
		v.y=-u.z;
		v.z=u.y;
	}
	else if(minCoordinate=='y')
	{
		v.x=-u.z;
		v.y=0.0;
		v.z=u.x;
	}
	else
	{
		v.x=-u.y;
		v.y=u.x;
		v.z=0.0;
	}
	v = normalizeVec3(v);
	w = crossProductVec3(u,v);
	w = normalizeVec3(w);
	Matrix[0][0]=u.x;
	Matrix[0][1]=u.y;
	Matrix[0][2]=u.z;
	Matrix[0][3]=0.0;
	Matrix[1][0]=v.x;
	Matrix[1][1]=v.y;
	Matrix[1][2]=v.z;
	Matrix[1][3]=0.0;
	Matrix[2][0]=w.x;
	Matrix[2][1]=w.y;
	Matrix[2][2]=w.z;
	Matrix[2][3]=0.0;
	Matrix[3][0]=0.0;
	Matrix[3][1]=0.0;
	Matrix[3][2]=0.0;
	Matrix[3][3]=1.0;

    InverseMatrix[0][0]=u.x;
	InverseMatrix[0][1]=v.x;
	InverseMatrix[0][2]=w.x;
	InverseMatrix[0][3]=0.0;
	InverseMatrix[1][0]=u.y;
	InverseMatrix[1][1]=v.y;
	InverseMatrix[1][2]=w.y;
	InverseMatrix[1][3]=0.0;
	InverseMatrix[2][0]=u.z;
	InverseMatrix[2][1]=v.z;
	InverseMatrix[2][2]=w.z;
	InverseMatrix[2][3]=0.0;
	InverseMatrix[3][0]=0.0;
	InverseMatrix[3][1]=0.0;
	InverseMatrix[3][2]=0.0;
	InverseMatrix[3][3]=1.0;


	Rotation[0][0]=1.0;
	Rotation[0][1]=0.0;
	Rotation[0][2]=0.0;
	Rotation[0][3]=0.0;
	Rotation[1][0]=0.0;
	Rotation[1][1]=cos(angle);
	Rotation[1][2]=-sin(angle);
	Rotation[1][3]=0.0;
	Rotation[2][0]=0.0;
	Rotation[2][1]=sin(angle);
	Rotation[2][2]=cos(angle);
	Rotation[2][3]=0.0;
	Rotation[3][0]=0.0;
	Rotation[3][1]=0.0;
	Rotation[3][2]=0.0;
	Rotation[3][3]=1.0;
}

void rotateCamera(double Matrix[4][4], Vec3 u, Vec3 v, Vec3 w)
{
	Matrix[0][0]=u.x;
	Matrix[0][1]=u.y;
	Matrix[0][2]=u.z;
	Matrix[0][3]=0.0;
	Matrix[1][0]=v.x;
	Matrix[1][1]=v.y;
	Matrix[1][2]=v.z;
	Matrix[1][3]=0.0;
	Matrix[2][0]=w.x;
	Matrix[2][1]=w.y;
	Matrix[2][2]=w.z;
	Matrix[2][3]=0.0;
	Matrix[3][0]=0.0;
	Matrix[3][1]=0.0;
	Matrix[3][2]=0.0;
	Matrix[3][3]=1.0;
}


void copyMatrix(double M1[4][4],double M2[4][4])
{
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			M1[i][j]=M2[i][j];
		}

	}
}
/*
	Transformations, culling, rasterization are done here.
	You can define helper functions inside this file (rasterizer.c) only.
	Using types in "hw2_types.h" and functions in "hw2_math_ops.c" will speed you up while working.
*/
void forwardRenderingPipeline(Camera cam) {
    // TODO: IMPLEMENT HERE
    int i,j,k,transformationID;
    double angle,translate[4][4],scale[4][4],TransformationMatrix[4][4],ResultingMatrix[4][4],M[4][4],MInverse[4][4],R[4][4];
    Vec3 u;

    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++) {
			TransformationMatrix[i][j]=0.1;
			ResultingMatrix[i][j]=0.1;
        }

    //for(i = 0; i < numberOfModels; i++)
    for(i = 0; i < numberOfModels; i++)
	{
    	makeIdentityMatrix(TransformationMatrix);
   		makeIdentityMatrix(ResultingMatrix);

    	for (j = 0; j < models[i].numberOfTransformations; j++) {
    		transformationID=models[i].transformationIDs[j];
			switch(models[i].transformationTypes[j])
            {
            	case 't':
            		performTranslation(translate,translations[transformationID].tx,translations[transformationID].ty,translations[transformationID].tz);
            		multiplyMatrixWithMatrix(TransformationMatrix, translate, ResultingMatrix);
            		copyMatrix(ResultingMatrix,TransformationMatrix);
            		break;
            	case 's':
            		performScaling(scale,scalings[transformationID].sx,scalings[transformationID].sy,scalings[transformationID].sz);
            		multiplyMatrixWithMatrix(TransformationMatrix, scale, ResultingMatrix);
            		copyMatrix(ResultingMatrix,TransformationMatrix);
					break;
            	case 'r':
            		angle=rotations[transformationID].angle;
            		u.x=rotations[transformationID].ux;
            		u.y=rotations[transformationID].uy;
            		u.z=rotations[transformationID].uz;
         		    performRotation(M,MInverse,R,angle,u);

         		    multiplyMatrixWithMatrix(TransformationMatrix, M, ResultingMatrix);
         		    copyMatrix(ResultingMatrix,TransformationMatrix);
         		    multiplyMatrixWithMatrix(TransformationMatrix, R, ResultingMatrix);
         		    copyMatrix(ResultingMatrix,TransformationMatrix);
         		    multiplyMatrixWithMatrix(TransformationMatrix, MInverse, ResultingMatrix);
         		    copyMatrix(ResultingMatrix,TransformationMatrix);

            		break;
			}
        }


        double cameraTranslation[4][4],cameraRotation[4][4],projection[4][4],viewPortTransformation[4][4];

		performTranslation(cameraTranslation,-cam.pos.x,-cam.pos.y,-cam.pos.z);
		multiplyMatrixWithMatrix(TransformationMatrix, cameraTranslation, ResultingMatrix);
        copyMatrix(ResultingMatrix,TransformationMatrix);

		rotateCamera(cameraRotation,cam.u,cam.v,cam.w);
        multiplyMatrixWithMatrix(TransformationMatrix, cameraRotation, ResultingMatrix);
        copyMatrix(ResultingMatrix,TransformationMatrix);

        projection[0][0]=(2.0*cam.n)/(cam.r-cam.l);
		projection[0][1]=0.0;
		projection[0][2]=(cam.r+cam.l)/(cam.r-cam.l);
		projection[0][3]=0.0;
		projection[1][0]=0.0;
		projection[1][1]=(2.0*cam.n)/(cam.t-cam.b);
		projection[1][2]=(cam.t+cam.b)/(cam.t-cam.b);
		projection[1][3]=0.0;
		projection[2][0]=0.0;
		projection[2][1]=0.0;
		projection[2][2]=-((cam.f+cam.n)/(cam.f-cam.n));
		projection[2][3]=-((2.0*cam.n*cam.f)/(cam.f-cam.n));
		projection[3][0]=0.0;
		projection[3][1]=0.0;
		projection[3][2]=-1.0;
		projection[3][3]=0.0;

        multiplyMatrixWithMatrix(TransformationMatrix, projection, ResultingMatrix);
        copyMatrix(ResultingMatrix,TransformationMatrix);




        viewPortTransformation[0][0]=cam.sizeX/2.0;
		viewPortTransformation[0][1]=0.0;
		viewPortTransformation[0][2]=0.0;
		viewPortTransformation[0][3]=((cam.sizeX-1)/2.0);
		viewPortTransformation[1][0]=0.0;
		viewPortTransformation[1][1]=cam.sizeY/2.0;
		viewPortTransformation[1][2]=0.0;
		viewPortTransformation[1][3]=((cam.sizeY-1)/2.0);
		viewPortTransformation[2][0]=0.0;
		viewPortTransformation[2][1]=0.0;
		viewPortTransformation[2][2]=1.0/2.0;
		viewPortTransformation[2][3]=1.0/2.0;
		viewPortTransformation[3][0]=0.0;
		viewPortTransformation[3][1]=0.0;
		viewPortTransformation[3][2]=0.0;
		viewPortTransformation[3][3]=1.0;




		Triangle4 trianglesWHC[1];
		double vertex[4],resultVertex[4];
		int counter=0;


		for (k = 0; k < models[i].numberOfTriangles; k++) {

			vertex[0]=vertices[models[i].triangles[k].vertexIds[0]].x;
		    vertex[1]=vertices[models[i].triangles[k].vertexIds[0]].y;
		    vertex[2]=vertices[models[i].triangles[k].vertexIds[0]].z;
			vertex[3]=1.0;

			multiplyMatrixWithVec4d(resultVertex,TransformationMatrix,vertex);

			vertex[0]=resultVertex[0]/resultVertex[3];
			vertex[1]=resultVertex[1]/resultVertex[3];
			vertex[2]=resultVertex[2]/resultVertex[3];
			vertex[3]=resultVertex[3]/resultVertex[3];

			multiplyMatrixWithVec4d(resultVertex,viewPortTransformation,vertex);

			trianglesWHC[0].vertices4[0].x=resultVertex[0];
			trianglesWHC[0].vertices4[0].y=resultVertex[1];
			trianglesWHC[0].vertices4[0].z=resultVertex[2];
			trianglesWHC[0].vertices4[0].w=resultVertex[3];
			trianglesWHC[0].vertices4[0].colorId=vertices[models[i].triangles[k].vertexIds[0]].colorId;






		    vertex[0]=vertices[models[i].triangles[k].vertexIds[1]].x;
		    vertex[1]=vertices[models[i].triangles[k].vertexIds[1]].y;
		    vertex[2]=vertices[models[i].triangles[k].vertexIds[1]].z;
			vertex[3]=1.0;

			multiplyMatrixWithVec4d(resultVertex,TransformationMatrix,vertex);

			vertex[0]=resultVertex[0]/resultVertex[3];
			vertex[1]=resultVertex[1]/resultVertex[3];
			vertex[2]=resultVertex[2]/resultVertex[3];
			vertex[3]=resultVertex[3]/resultVertex[3];

			multiplyMatrixWithVec4d(resultVertex,viewPortTransformation,vertex);

			trianglesWHC[0].vertices4[1].x=resultVertex[0];
			trianglesWHC[0].vertices4[1].y=resultVertex[1];
			trianglesWHC[0].vertices4[1].z=resultVertex[2];
			trianglesWHC[0].vertices4[1].w=resultVertex[3];
			trianglesWHC[0].vertices4[1].colorId=vertices[models[i].triangles[k].vertexIds[1]].colorId;






			vertex[0]=vertices[models[i].triangles[k].vertexIds[2]].x;
		    vertex[1]=vertices[models[i].triangles[k].vertexIds[2]].y;
		    vertex[2]=vertices[models[i].triangles[k].vertexIds[2]].z;
			vertex[3]=1.0;

			multiplyMatrixWithVec4d(resultVertex,TransformationMatrix,vertex);

			vertex[0]=resultVertex[0]/resultVertex[3];
			vertex[1]=resultVertex[1]/resultVertex[3];
			vertex[2]=resultVertex[2]/resultVertex[3];
			vertex[3]=resultVertex[3]/resultVertex[3];

			multiplyMatrixWithVec4d(resultVertex,viewPortTransformation,vertex);

			trianglesWHC[0].vertices4[2].x=resultVertex[0];
			trianglesWHC[0].vertices4[2].y=resultVertex[1];
			trianglesWHC[0].vertices4[2].z=resultVertex[2];
			trianglesWHC[0].vertices4[2].w=resultVertex[3];
			trianglesWHC[0].vertices4[2].colorId=vertices[models[i].triangles[k].vertexIds[2]].colorId;

			// if(backfaceCullingSetting==1)
               int flag = 0;
			if(1)
			{
				Vec3 normal,v;
				Vec3 vector1,vector2,center;
				double value,L0,L1,L2;

				vector1.x=trianglesWHC[0].vertices4[0].x-trianglesWHC[0].vertices4[1].x;
				vector1.y=trianglesWHC[0].vertices4[0].y-trianglesWHC[0].vertices4[1].y;
				vector1.z=trianglesWHC[0].vertices4[0].z-trianglesWHC[0].vertices4[1].z;
				vector1=normalizeVec3(vector1);
				vector2.x=trianglesWHC[0].vertices4[2].x-trianglesWHC[0].vertices4[1].x;
				vector2.y=trianglesWHC[0].vertices4[2].y-trianglesWHC[0].vertices4[1].y;
				vector2.z=trianglesWHC[0].vertices4[2].z-trianglesWHC[0].vertices4[1].z;
				vector2=normalizeVec3(vector2);

				v.x=trianglesWHC[0].vertices4[1].x-cam.pos.x;
				v.y=trianglesWHC[0].vertices4[1].y-cam.pos.y;
				v.z=trianglesWHC[0].vertices4[1].z-cam.pos.z;
				v=normalizeVec3(v);

				normal=crossProductVec3(vector2,vector1);
				normal=normalizeVec3(normal);
				value=dotProductVec3(normal,v);


				if(value>0.00001)
				{
					continue;
                         flag = 1;
				}
			}
               if(!flag){

                    midPointAlgorithm(convertXIndices(trianglesWHC[0].vertices4[0].x,cam),convertYIndices(trianglesWHC[0].vertices4[0].y,cam),colors[trianglesWHC[0].vertices4[0].colorId],convertXIndices(trianglesWHC[0].vertices4[1].x,cam),convertYIndices(trianglesWHC[0].vertices4[1].y,cam),colors[trianglesWHC[0].vertices4[1].colorId]);
                    midPointAlgorithm(convertXIndices(trianglesWHC[0].vertices4[1].x,cam),convertYIndices(trianglesWHC[0].vertices4[1].y,cam),colors[trianglesWHC[0].vertices4[1].colorId],convertXIndices(trianglesWHC[0].vertices4[2].x,cam),convertYIndices(trianglesWHC[0].vertices4[2].y,cam),colors[trianglesWHC[0].vertices4[2].colorId]);
                    midPointAlgorithm(convertXIndices(trianglesWHC[0].vertices4[2].x,cam),convertYIndices(trianglesWHC[0].vertices4[2].y,cam),colors[trianglesWHC[0].vertices4[2].colorId],convertXIndices(trianglesWHC[0].vertices4[0].x,cam),convertYIndices(trianglesWHC[0].vertices4[0].y,cam),colors[trianglesWHC[0].vertices4[0].colorId]);

                    if(models[i].type==1)
                    {
                         fillInTriangle(convertXIndices(trianglesWHC[0].vertices4[0].x,cam),convertYIndices(trianglesWHC[0].vertices4[0].y,cam),colors[trianglesWHC[0].vertices4[0].colorId],convertXIndices(trianglesWHC[0].vertices4[1].x,cam),convertYIndices(trianglesWHC[0].vertices4[1].y,cam),colors[trianglesWHC[0].vertices4[1].colorId],convertXIndices(trianglesWHC[0].vertices4[2].x,cam),convertYIndices(trianglesWHC[0].vertices4[2].y,cam),colors[trianglesWHC[0].vertices4[2].colorId]);
                    }

                    counter++;
               }
        }


        /*
        int IndexOfLastElement=counter;
        double dx,dy;
        for (k = 0; k < counter; k++)
        {
    		midPointAlgorithm(convertXIndices(trianglesWHC[k].vertices4[0].x,cam),convertYIndices(trianglesWHC[k].vertices4[0].y,cam),colors[trianglesWHC[k].vertices4[0].colorId],convertXIndices(trianglesWHC[k].vertices4[1].x,cam),convertYIndices(trianglesWHC[k].vertices4[1].y,cam),colors[trianglesWHC[k].vertices4[1].colorId]);
			midPointAlgorithm(convertXIndices(trianglesWHC[k].vertices4[1].x,cam),convertYIndices(trianglesWHC[k].vertices4[1].y,cam),colors[trianglesWHC[k].vertices4[1].colorId],convertXIndices(trianglesWHC[k].vertices4[2].x,cam),convertYIndices(trianglesWHC[k].vertices4[2].y,cam),colors[trianglesWHC[k].vertices4[2].colorId]);
    		midPointAlgorithm(convertXIndices(trianglesWHC[k].vertices4[2].x,cam),convertYIndices(trianglesWHC[k].vertices4[2].y,cam),colors[trianglesWHC[k].vertices4[2].colorId],convertXIndices(trianglesWHC[k].vertices4[0].x,cam),convertYIndices(trianglesWHC[k].vertices4[0].y,cam),colors[trianglesWHC[k].vertices4[0].colorId]);
			if(models[i].type==1)
			{
				fillInTriangle(convertXIndices(trianglesWHC[k].vertices4[0].x,cam),convertYIndices(trianglesWHC[k].vertices4[0].y,cam),colors[trianglesWHC[k].vertices4[0].colorId],convertXIndices(trianglesWHC[k].vertices4[1].x,cam),convertYIndices(trianglesWHC[k].vertices4[1].y,cam),colors[trianglesWHC[k].vertices4[1].colorId],convertXIndices(trianglesWHC[k].vertices4[2].x,cam),convertYIndices(trianglesWHC[k].vertices4[2].y,cam),colors[trianglesWHC[k].vertices4[2].colorId]);
			}
		}
		*/

}
}

void fillInTriangle(int x0,int y0,Color c0,int x1,int y1,Color c1,int x2,int y2,Color c2)
{
	int x,y,xmin,ymin,xmax,ymax;
	double alpha,beta,gamma;
	Color c;
	xmin=x0;
	ymin=y0;
	xmax=x0;
	ymax=y0;
	if(x1<xmin)
	{
		xmin=x1;
	}
	if(x2<xmin)
	{
		xmin=x2;
	}
	if(y1<ymin)
	{
		ymin=y1;
	}
	if(y2<ymin)
	{
		ymin=y2;
	}

	if(x1>xmax)
	{
		xmax=x1;
	}
	if(x2>xmax)
	{
		xmax=x2;
	}
	if(y1>ymax)
	{
		ymax=y1;
	}
	if(y2>ymax)
	{
		ymax=y2;
	}
	for(y=ymin;y<=ymax;y++)
	{
		for(x=xmin;x<=xmax;x++)
		{
			alpha=(1.0*(y1-y2)*(x-x1)+(x2-x1)*(y-y1))/((y1-y2)*(x0-x1)+(x2-x1)*(y0-y1));
			beta=(1.0*(y2-y0)*(x-x2)+(x0-x2)*(y-y2))/((y2-y0)*(x1-x2)+(x0-x2)*(y1-y2));
			gamma=(1.0*(y0-y1)*(x-x0)+(x1-x0)*(y-y0))/((y0-y1)*(x2-x0)+(x1-x0)*(y2-y0));
			if((alpha>=0.0)&&(beta>=0.0)&&(gamma>=0.0))
			{
				image[x][y].r=alpha*c0.r+beta*c1.r+gamma*c2.r;
				image[x][y].g=alpha*c0.g+beta*c1.g+gamma*c2.g;
				image[x][y].b=alpha*c0.b+beta*c1.b+gamma*c2.b;
			}
		}
	}
}

void midPointAlgorithm(int x0,int y0,Color c0,int x1,int y1,Color c1)
{
	int dx,dy;
	double m;
	int x,y,d,tempx,tempy;

	dx=x1-x0;
	dy=y1-y0;
	Color dc;
	Color c,tempc;

	if(dx==0)
	{
		if(y1<y0)
		{
			tempy=y0;
			y0=y1;
			y1=tempy;

			tempc.r=c0.r;
			tempc.g=c0.g;
			tempc.b=c0.b;

			c0.r=c1.r;
			c0.g=c1.g;
			c0.b=c1.b;

			c1.r=tempc.r;
			c1.g=tempc.g;
			c1.b=tempc.b;
		}

		dy= ABS(dy);
		dc.r=(c1.r-c0.r)/dy;
		dc.g=(c1.g-c0.g)/dy;
		dc.b=(c1.b-c0.b)/dy;

		c.r=c0.r;
		c.g=c0.g;
		c.b=c0.b;

		x=x0;
		for(y=y0;y<=y1;y++)
		{
			image[x][y].r=c.r;
			image[x][y].g=c.g;
			image[x][y].b=c.b;
			c.r+=dc.r;
			c.g+=dc.g;
			c.b+=dc.b;
		}
	}
	else if(dy==0)
	{

		if(x1<x0)
		{
			tempx=x0;
			x0=x1;
			x1=tempx;

			tempc.r=c0.r;
			tempc.g=c0.g;
			tempc.b=c0.b;

			c0.r=c1.r;
			c0.g=c1.g;
			c0.b=c1.b;

			c1.r=tempc.r;
			c1.g=tempc.g;
			c1.b=tempc.b;
		}

		dx=ABS(dx);
		dc.r=(c1.r-c0.r)/dx;
		dc.g=(c1.g-c0.g)/dx;
		dc.b=(c1.b-c0.b)/dx;

		c.r=c0.r;
		c.g=c0.g;
		c.b=c0.b;

		y=y0;
		for(x=x0;x<=x1;x++)
		{
			image[x][y].r=c.r;
			image[x][y].g=c.g;
			image[x][y].b=c.b;
			c.r+=dc.r;
			c.g+=dc.g;
			c.b+=dc.b;
		}
	}
	else{
		m=(1.0*dy)/(1.0*dx);
		if((m>0.0)&&(m<=1.0))
		{
			if((dy<0)&&(dx<0))
				{
					tempx=x0;
					tempy=y0;
					x0=x1;
					y0=y1;
					x1=tempx;
					y1=tempy;

					tempc.r=c0.r;
					tempc.g=c0.g;
					tempc.b=c0.b;

					c0.r=c1.r;
					c0.g=c1.g;
					c0.b=c1.b;

					c1.r=tempc.r;
					c1.g=tempc.g;
					c1.b=tempc.b;
				}
				dx=ABS(dx);
			dc.r=(c1.r-c0.r)/dx;
			dc.g=(c1.g-c0.g)/dx;
			dc.b=(c1.b-c0.b)/dx;

			c.r=c0.r;
			c.g=c0.g;
			c.b=c0.b;

			y=y0;
			d=2*(y0-y1)+(x1-x0);
			for(x=x0;x<=x1;x++)
			{
				image[x][y].r=c.r;
				image[x][y].g=c.g;
				image[x][y].b=c.b;
				if(d<0)
				{
					y=y+1;
					d+=2*((y0-y1)+(x1-x0));
				}
				else
				{
					d+=2*(y0-y1);
				}
				c.r+=dc.r;
				c.g+=dc.g;
				c.b+=dc.b;
			}
		}
		else if(m>1.0)
		{
			if((dy<0)&&(dx<0))
				{
					tempx=x0;
					tempy=y0;
					x0=x1;
					y0=y1;
					x1=tempx;
					y1=tempy;

					tempc.r=c0.r;
					tempc.g=c0.g;
					tempc.b=c0.b;

					c0.r=c1.r;
					c0.g=c1.g;
					c0.b=c1.b;

					c1.r=tempc.r;
					c1.g=tempc.g;
					c1.b=tempc.b;
				}
				dy=ABS(dy);
			dc.r=(c1.r-c0.r)/dy;
			dc.g=(c1.g-c0.g)/dy;
			dc.b=(c1.b-c0.b)/dy;

			c.r=c0.r;
			c.g=c0.g;
			c.b=c0.b;

			x=x0;
			d=(y0-y1)+2*(x1-x0);
			for(y=y0;y<=y1;y++)
			{
				image[x][y].r=c.r;
				image[x][y].g=c.g;
				image[x][y].b=c.b;
				if(d<0)
				{
				//	printf("heloo");
					d+=2*(x1-x0);
				}
				else
				{

					x=x+1;
					d+=2*((y0-y1)+(x1-x0));
				}
				c.r+=dc.r;
				c.g+=dc.g;
				c.b+=dc.b;
			}
		}
		else if((m>=-1.0)&&(m<0))
			{
				if(dy<0)
				{
					tempx=x0;
					tempy=y0;
					x0=x1;
					y0=y1;
					x1=tempx;
					y1=tempy;

					tempc.r=c0.r;
					tempc.g=c0.g;
					tempc.b=c0.b;

					c0.r=c1.r;
					c0.g=c1.g;
					c0.b=c1.b;

					c1.r=tempc.r;
					c1.g=tempc.g;
					c1.b=tempc.b;
				}
				dx=ABS(dx);
				dc.r=(c1.r-c0.r)/dx;
				dc.g=(c1.g-c0.g)/dx;
				dc.b=(c1.b-c0.b)/dx;

				c.r=c0.r;
				c.g=c0.g;
				c.b=c0.b;

				y=y0;
				d=-2*(y0-y1)+(x1-x0);
				for(x=x0;x>=x1;x--)
				{
					image[x][y].r=c.r;
					image[x][y].g=c.g;
					image[x][y].b=c.b;
					if(d>0)
					{
						y=y+1;
						d+=2*(-1*(y0-y1)+(x1-x0));
					}
					else
					{
						d-=2*(y0-y1);
					}
					c.r+=dc.r;
					c.g+=dc.g;
					c.b+=dc.b;
				}
			}
			else if(m<-1.0)
			{
				if(dy<0)
				{
					tempx=x0;
					tempy=y0;
					x0=x1;
					y0=y1;
					x1=tempx;
					y1=tempy;

					tempc.r=c0.r;
					tempc.g=c0.g;
					tempc.b=c0.b;

					c0.r=c1.r;
					c0.g=c1.g;
					c0.b=c1.b;

					c1.r=tempc.r;
					c1.g=tempc.g;
					c1.b=tempc.b;
				}
				dy=ABS(dy);
				dc.r=(c1.r-c0.r)/dy;
				dc.g=(c1.g-c0.g)/dy;
				dc.b=(c1.b-c0.b)/dy;

				c.r=c0.r;
				c.g=c0.g;
				c.b=c0.b;

				x=x0;
				d=-(y0-y1)+2*(x1-x0);
				for(y=y0;y<=y1;y++)
				{
					image[x][y].r=c.r;
					image[x][y].g=c.g;
					image[x][y].b=c.b;
					if(d<0)
					{
						x=x-1;
						d+=2*(-1*(y0-y1)+(x1-x0));
					}
					else
					{
						d+=2*(x1-x0);
					}
					c.r+=dc.r;
					c.g+=dc.g;
					c.b+=dc.b;
				}
			}

	}

}



void initializeImage(Camera cam) {
    int i, j;

    for (i = 0; i < cam.sizeX; i++)
        for (j = 0; j < cam.sizeY; j++) {
            image[i][j].r = backgroundColor.r;
            image[i][j].g = backgroundColor.g;
            image[i][j].b = backgroundColor.b;

        }
}

void readCameraFile(char *camFileName) {
    FILE *fp;
    int i = 0;
    char line[80] = {};
    fp = fopen(camFileName, "r");

    if (fp == NULL) {
        printf("ERROR: Can't open %s\n", camFileName);
        exit(1);
    }

    /* read number of cameras */
    fscanf(fp, "%d", &numberOfCameras);

    for (i = 0; i < numberOfCameras; i++) {
        /* skip line "#Camera n" */
        fscanf(fp, "%s %d", line, &(cameras[i].cameraId));
        fscanf(fp, "%lf %lf %lf", &(cameras[i].pos.x), &(cameras[i].pos.y), &(cameras[i].pos.z));
        fscanf(fp, "%lf %lf %lf", &(cameras[i].gaze.x), &(cameras[i].gaze.y), &(cameras[i].gaze.z));
        fscanf(fp, "%lf %lf %lf", &(cameras[i].v.x), &(cameras[i].v.y), &(cameras[i].v.z));

        cameras[i].gaze = normalizeVec3(cameras[i].gaze);
        cameras[i].u = crossProductVec3(cameras[i].gaze, cameras[i].v);
        cameras[i].u = normalizeVec3(cameras[i].u);

        cameras[i].w.x = -cameras[i].gaze.x;
        cameras[i].w.y = -cameras[i].gaze.y;
        cameras[i].w.z = -cameras[i].gaze.z;
        cameras[i].v = crossProductVec3(cameras[i].u, cameras[i].gaze);
        cameras[i].v = normalizeVec3(cameras[i].v);


        fscanf(fp, "%lf %lf %lf %lf", &(cameras[i].l), &(cameras[i].r), &(cameras[i].b), &(cameras[i].t));
        fscanf(fp, "%lf", &(cameras[i].n));
        fscanf(fp, "%lf", &(cameras[i].f));
        fscanf(fp, "%d %d", &(cameras[i].sizeX), &(cameras[i].sizeY));
        fscanf(fp, "%s", cameras[i].outputFileName);

    }
}

/*
 * Reads scene file.
 */
void readSceneFile(char *sceneFileName) {
    FILE *fp;
    char line[80];
    int i, j;
    char tmp[80];

    fp = fopen(sceneFileName, "r");

    if (fp == NULL) {
        printf("ERROR: Can't open %s\n", sceneFileName);
        exit(1);
    }

    /* read background color */
    fscanf(fp, "%lf %lf %lf", &(backgroundColor.r), &(backgroundColor.g), &(backgroundColor.b));

    /* read backface culling setting, enabled-disabled */
    fscanf(fp, "%d", &(backfaceCullingSetting));

    /* skip line "#Vertices" */
    fscanf(fp, "%s", line);

    /* read number of vertices */
    fscanf(fp, "%d", &numberOfVertices);
    numberOfColors = numberOfVertices;

    /* skip line "#Colors" */
    fscanf(fp, "%s", line);

    for (i = 1; i <= numberOfColors; i++) {
        fscanf(fp, "%lf %lf %lf", &(colors[i].r), &(colors[i].g), &(colors[i].b));
    }

    /* skip line "#Positions" */
    fscanf(fp, "%s", line);

    for (i = 1; i <= numberOfVertices; i++) {
        fscanf(fp, "%lf %lf %lf", &(vertices[i].x), &(vertices[i].y), &(vertices[i].z));
        vertices[i].colorId = i;
    }

    /* skip line "#Translations" */
    fscanf(fp, "%s", line);

    /* read number of translations */
    fscanf(fp, "%d", &numberOfTranslations);

    for (i = 1; i <= numberOfTranslations; i++) {
        fscanf(fp, "%lf %lf %lf", &(translations[i].tx), &(translations[i].ty), &(translations[i].tz));
    }

    /* skip line "#Scalings" */
    fscanf(fp, "%s", line);

    /* read number of scalings */
    fscanf(fp, "%d", &numberOfScalings);

    for (i = 1; i <= numberOfScalings; i++) {
        fscanf(fp, "%lf %lf %lf", &(scalings[i].sx), &(scalings[i].sy), &(scalings[i].sz));
    }

    /* skip line "#Rotations" */
    fscanf(fp, "%s", line);

    /* read number of rotations */
    fscanf(fp, "%d", &numberOfRotations);

    for (i = 1; i <= numberOfRotations; i++) {
        fscanf(fp, "%lf %lf %lf %lf", &(rotations[i].angle), &(rotations[i].ux), &(rotations[i].uy),
               &(rotations[i].uz));
    }

    /* skip line "#Models" */
    fscanf(fp, "%s", line);

    /* read number of models */
    fscanf(fp, "%d", &numberOfModels);

    for (i = 0; i < numberOfModels; i++) {
        /* read model id */
        fscanf(fp, "%d", &(models[i].modelId));

        /* read model type */
        fscanf(fp, "%d", &(models[i].type));

         /* read number of transformations */
        fscanf(fp, "%d", &(models[i].numberOfTransformations));

        for (j = 0; j < models[i].numberOfTransformations; j++) {
            fscanf(fp, "%s", tmp);
            models[i].transformationTypes[j] = tmp[0];
            fscanf(fp, "%d", &(models[i].transformationIDs[j]));
        }

        /* read number of triangles */
        fscanf(fp, "%d", &(models[i].numberOfTriangles));

        for (j = 0; j < models[i].numberOfTriangles; j++) {
            fscanf(fp, "%d %d %d", &(models[i].triangles[j].vertexIds[0]), &(models[i].triangles[j].vertexIds[1]),
                   &(models[i].triangles[j].vertexIds[2]));

        }

    }

    fclose(fp);

}

/*
 * If given value is less than 0, converts value to 0.
 * If given value is more than 255, converts value to 255.
 * Otherwise returns value itself.
 */
int make_between_0_255(double value) {
    if (value >= 255.0) return 255;
    if (value <= 0.0) return 0;
    return (int) (value);
}

/*
 * Writes contents of image (Color**) into a PPM file.
 */
void writeImageToPPMFile(Camera camera) {
    FILE *outputFile;
    int i, j;

    outputFile = fopen(camera.outputFileName, "w");

    fprintf(outputFile, "P3\n");
    fprintf(outputFile, "# %s\n", camera.outputFileName);

    fprintf(outputFile, "%d %d\n", camera.sizeX, camera.sizeY);

    fprintf(outputFile, "255\n");

    for (j = camera.sizeY - 1; j >= 0; j--) {
        for (i = 0; i < camera.sizeX; i++) {
            fprintf(outputFile, "%d %d %d ", make_between_0_255(image[i][j].r), make_between_0_255(image[i][j].g), make_between_0_255(image[i][j].b));
        }
        fprintf(outputFile, "\n");
    }
    fclose(outputFile);
}


/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
	os_type == 1 		-> Ubuntu
	os_type == 2 		-> Windows
	os_type == other	-> No conversion
*/
void convertPPMToPNG(char *ppmFileName, int os_type) {
    char command[100];

    // call command on Ubuntu
    if (os_type == 1) {
        sprintf(command, "convert %s %s.png", ppmFileName, ppmFileName);
        system(command);
    }

        // call command on Windows
    else if (os_type == 2) {
        sprintf(command, "magick convert %s %s.png", ppmFileName, ppmFileName);
        system(command);
    }

        // default action - don't do conversion
    else {

    }
}
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
    printf("(%lf,%lf,%lf)\n", v.x, v.y, v.z);
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

void writeImage(Camera camera)
{
	FILE *outFile;
	int i,j;

	outFile = fopen(camera.outputFileName,"w");

	fprintf(outFile,"P3\n");
	fprintf(outFile,"# %s\n",camera.outputFileName);

	fprintf(outFile,"%d %d\n",camera.sizeX,camera.sizeY);

	fprintf(outFile,"255\n");

	for (j=camera.sizeY-1;j>=0;j--)
	{
		for (i=0;i<camera.sizeX;i++)
		{
			fprintf(outFile,"%d %d %d ",convert(image[i][j].r),convert(image[i][j].g),convert(image[i][j].b));
		}
		fprintf(outFile,"\n");
	}
	fclose(outFile);
}
int convert (double d)
{
	if (d>255) return 255;
	if (d<0) return 0;
	return round(d);
}

int convertXIndices (double d,Camera cam)
{
	if (d>cam.sizeX) return cam.sizeX;
	if (d<0) return 0;
	return round(d);
}
int convertYIndices (double d,Camera cam)
{
	if (d>cam.sizeY) return cam.sizeY;
	if (d<0) return 0;
	return round(d);
}

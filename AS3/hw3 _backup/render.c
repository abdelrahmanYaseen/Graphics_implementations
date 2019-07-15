#include "GLFW/glfw3.h"
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TARGET_FPS 30

GLuint program;
GLuint martyTextureMap;
GLuint rockTextureMap;
GLuint moldTextureMap;

typedef GLubyte Pixel[3];

typedef struct
{
	float x,y,z;
} Vec3;

typedef struct
{
        Vec3 p;
        Vec3 n;
        float s,t;
        int tCount;
} Vertex;

typedef struct
{
	int a,b,c; // vertex indices
	Vec3 normal;
} Triangle;


int numMeshes = 0;

Vertex vertices[20][20000];
int numVertices[20] = {0};

Triangle triangles[20][20000];
int numTriangles[20] = {0};

Vec3 cross(Vec3 a, Vec3 b)
{
	Vec3 tmp;

	tmp.x = a.y*b.z-b.y*a.z;
	tmp.y = b.x*a.z-a.x*b.z;
	tmp.z = a.x*b.y-b.x*a.y;

	return tmp;
}

float dot(Vec3 a, Vec3 b)
{
		return a.x*b.x+a.y*b.y+a.z*b.z;
}

float length2(Vec3 v)
{
	return (v.x*v.x+v.y*v.y+v.z*v.z);
}

float length(Vec3 v)
{
	return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

Vec3 normalize(Vec3 v)
{
	Vec3 tmp;
	float d;

	d=length(v);
	tmp.x = v.x/d;
	tmp.y = v.y/d;
	tmp.z = v.z/d;

	return tmp;
}

Vec3 add(Vec3 a, Vec3 b)
{
	Vec3 tmp;
	tmp.x = a.x+b.x;
	tmp.y = a.y+b.y;
	tmp.z = a.z+b.z;

	return tmp;
}

Vec3 mult(Vec3 a, float c)
{
	Vec3 tmp;
	tmp.x = a.x*c;
	tmp.y = a.y*c;
	tmp.z = a.z*c;

	return tmp;
}

void computeNormalVectors()
{
	int i,m;
	for (m=0;m<numMeshes;m++) {
	    for (i=0;i<numTriangles[m];i++)
	    {
		 triangles[m][i].normal = cross(add(vertices[m][triangles[m][i].b].p,mult(vertices[m][triangles[m][i].a].p,-1)),add(vertices[m][triangles[m][i].c].p,mult(vertices[m][triangles[m][i].a].p,-1)));

		 triangles[m][i].normal = normalize(triangles[m][i].normal);
        }
    }
}

void computeVertexNormals()
{
     int i,m;
	for (m=0;m<numMeshes;m++) {
      for (i=0;i<numVertices[m];i++)
      {
         vertices[m][i].tCount=0;
         vertices[m][i].n.x=vertices[m][i].n.y=vertices[m][i].n.y = 0;
      }
     }

	for (m=0;m<numMeshes;m++) {
     for (i=0;i<numTriangles[m];i++)
     {
         vertices[m][triangles[m][i].a].tCount++;
         vertices[m][triangles[m][i].b].tCount++;
         vertices[m][triangles[m][i].c].tCount++;
         vertices[m][triangles[m][i].a].n = add(vertices[m][triangles[m][i].a].n,triangles[m][i].normal);
         vertices[m][triangles[m][i].b].n = add(vertices[m][triangles[m][i].b].n,triangles[m][i].normal);
         vertices[m][triangles[m][i].c].n = add(vertices[m][triangles[m][i].c].n,triangles[m][i].normal);
      }
    }

	for (m=0;m<numMeshes;m++) {
        for (i=0;i<numVertices[m];i++)
         {
             vertices[m][i].n = mult(vertices[m][i].n,1.0/vertices[m][i].tCount);
         }
     }

}
void readInput(char *fileName)
{

     FILE *fp;
     int i;

     fp = fopen(fileName,"r");
     if (fp==NULL)
     {
         fprintf(stderr,"The input scene file %s cannot be opened!\n",fileName);
         exit(1);
     }
     char tmp[300];

    printf("Parsing Input Scene File....\r\n");

     while (fgets(tmp,300,fp)!=NULL)
     {
            sscanf(tmp,"%d",&(numVertices[numMeshes]));
            for (i=0;i<numVertices[numMeshes];i++)
            {
                        fgets(tmp,300,fp);
                        sscanf(tmp,"%f %f %f",&(vertices[numMeshes][i].p.x),&(vertices[numMeshes][i].p.y),&(vertices[numMeshes][i].p.z));
            }

            for (i=0;i<numVertices[numMeshes];i++)
            {
                        fgets(tmp,300,fp);
                        sscanf(tmp,"%f %f",&(vertices[numMeshes][i].s),&(vertices[numMeshes][i].t));
            }

            fgets(tmp,300,fp);
            sscanf(tmp,"%d",&(numTriangles[numMeshes]));

            for (i=0;i<numTriangles[numMeshes];i++)
            {
                        fgets(tmp,300,fp);
                        sscanf(tmp,"%d %d %d",&(triangles[numMeshes][i].a),&(triangles[numMeshes][i].b),&(triangles[numMeshes][i].c));
            }

            numMeshes++;
	}
	computeNormalVectors();
	computeVertexNormals();
}


void readShader(const char* fname, char *source)
{
	FILE *fp;
	fp = fopen(fname,"r");
	if (fp==NULL)
	{
		fprintf(stderr,"The shader file %s cannot be opened!\n",fname);
		glfwTerminate();
		exit(1);
	}
	char tmp[300];
	while (fgets(tmp,300,fp)!=NULL)
	{
		strcat(source,tmp);
		strcat(source,"\n");
	}
}

unsigned int loadShader(const char *source, unsigned int mode)
{
	GLuint id;
	id = glCreateShader(mode);

	glShaderSource(id,1,&source,NULL);

	glCompileShader(id);

	char error[1000];

	glGetShaderInfoLog(id,1000,NULL,error);
	printf("Compile status: \n %s\n",error);

	return id;
}

void initShaders()
{
	char *vsSource, *fsSource;
	GLuint vs,fs;

	vsSource = (char *)malloc(sizeof(char)*20000);
	fsSource = (char *)malloc(sizeof(char)*20000);

	vsSource[0]='\0';
	fsSource[0]='\0';

	program = glCreateProgram();

	readShader("vertex.vs",vsSource);
	readShader("fragment.fs",fsSource);

	vs = loadShader(vsSource,GL_VERTEX_SHADER);
	fs = loadShader(fsSource,GL_FRAGMENT_SHADER);

	glAttachShader(program,vs);
	glAttachShader(program,fs);

	glLinkProgram(program);

	glUseProgram(program);

}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

vec3 LIGHTPOS;
float Mode = -1.0;
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
		LIGHTPOS[1]+=Mode*2;
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
		LIGHTPOS[0]+=Mode*2;
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		LIGHTPOS[2]+=Mode*2;
	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		Mode*=-1.0;

	printf("%1.1lf ",LIGHTPOS[0]);
	printf("%1.1lf ",LIGHTPOS[1]);
	printf("%1.1lf \n",LIGHTPOS[2]);
	printf("M: %1.1lf \n",Mode);

}

GLuint loadTexture_ASCII(const char* tname){
/*reads the image file assumes ppm format*/

	GLuint texture;
	int w,h,max;
	int Width, Height;
	int i,j;
	unsigned int r,g,b;
	int k;
	char ch;
	FILE *fp;
     Pixel *image;

	fp = fopen(tname,"r");

	printf("filename = %s\n",tname);

	/*read the header*/

    	fscanf(fp, "P%c\n", &ch);
	if (ch != '3') {
		fprintf(stderr, "Only ascii mode 3 channel PPM files");
		exit(-1);
	}

	/*strip comment lines*/
	ch = getc(fp);
	while (ch == '#') {
        	do {
		  ch = getc(fp);
        	}
		while (ch != '\n');

      		ch = getc(fp);
    	}
	ungetc(ch, fp);

	/*read the width*/
	fscanf(fp,"%d",&w);

	/*read the height*/
	fscanf(fp,"%d",&h);

	/*max intensity - not used here*/
	fscanf(fp,"%d",&max);

	Width = w;
	Height = h;

	printf("Width = %d, Height = %d\n",Width,Height);

	image = (Pixel *)malloc(Width*Height*sizeof(Pixel));

	for(i=0;i<Height;++i){
		for(j=0;j<Width;++j) {
			fscanf(fp,"%d %d %d",&r,&g,&b);
			k = i*Width+j; /*ok, can be more efficient here!*/
			(*(image+k))[0] = (GLubyte)r;
			(*(image+k))[1] = (GLubyte)g;
			(*(image+k))[2] = (GLubyte)b;
		}
	}
	fclose(fp);

    	glGenTextures(1,&texture);
    	glBindTexture(GL_TEXTURE_2D,texture);
    	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    	glBindTexture(GL_TEXTURE_2D,0);

    	free(image);

    	return texture;
}


int main(void)
{
    vec3 cam = {1,2,3};
    vec3 ref = {0,0,0};
    vec3 up = {0,1,0};
    vec3 LIGHTPOS = {5,20,5};

    GLFWwindow* window;

    GLuint vertex_buffer;
    GLuint normal_buffer;
    GLuint tcoord_buffer;
    GLuint index_buffer;

    GLfloat *vdata;
    GLfloat *tdata;
    GLfloat *ndata;
    GLuint *idata;
    int totalNumVertices = 0;
    int totalNumTriangles = 0;
    int i,j,c;

    GLint mvp_location, vpos_location, vtex_location, m_location,isMarty_location,lightPos_location;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(800, 400, "Render Object", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    readInput("martyandrocks.txt");

    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &normal_buffer);
    glGenBuffers(1, &tcoord_buffer);
    glGenBuffers(1, &index_buffer);


    for (i=0;i<numMeshes;i++)
    {
	totalNumVertices+= numVertices[i];
	totalNumTriangles+= numTriangles[i];
    }

    vdata = (GLfloat *)malloc(sizeof(GLfloat)*totalNumVertices*3);
    ndata = (GLfloat *)malloc(sizeof(GLfloat)*totalNumVertices*3);
    tdata = (GLfloat *)malloc(sizeof(GLfloat)*totalNumVertices*2);
    idata = (GLuint *)malloc(sizeof(GLuint)*totalNumTriangles*3);

    c = 0;
    for (i=0;i<numMeshes;i++)
    {
       for (j=0;j<numVertices[i];j++)
	{
		ndata[c] = vertices[i][j].n.x;
		vdata[c++]=vertices[i][j].p.x;
		ndata[c] = vertices[i][j].n.y;
		vdata[c++]=vertices[i][j].p.y;
		ndata[c] = vertices[i][j].n.z;
		vdata[c++]=vertices[i][j].p.z;
	}
    }

    c = 0;
    for (i=0;i<numMeshes;i++)
    {
       for (j=0;j<numVertices[i];j++)
	{
		tdata[c++]=vertices[i][j].s;
		tdata[c++]=vertices[i][j].t;
	}
    }

    c = 0;
    int offset = 0;

    for (i=0;i<numMeshes;i++)
    {
	for (j=0;j<numTriangles[i];j++)
	{
		idata[c++]=triangles[i][j].a+offset;
		idata[c++]=triangles[i][j].b+offset;
		idata[c++]=triangles[i][j].c+offset;
	}
	offset+=numVertices[i];
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*totalNumVertices, vdata, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*totalNumVertices, ndata, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, tcoord_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*2*totalNumVertices, tdata, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*totalNumTriangles*3, idata, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    free(vdata);
    free(idata);
    free(ndata);
    free(tdata);

    initShaders();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    martyTextureMap = loadTexture_ASCII("marty_texture.ppm");
    rockTextureMap = loadTexture_ASCII("rock_texture.ppm");
    moldTextureMap = loadTexture_ASCII("mold_texture.ppm");

    mvp_location = glGetUniformLocation(program, "MVP");
    m_location = glGetUniformLocation(program, "M");
    vpos_location = glGetUniformLocation(program, "eyePos");
    isMarty_location = glGetUniformLocation(program, "isMarty");
    lightPos_location = glGetUniformLocation(program, "lightPos");

    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        mat4x4 m, p, mvp,r;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
	glNormalPointer(GL_FLOAT,0,0);
	glBindBuffer(GL_ARRAY_BUFFER, tcoord_buffer);
	glTexCoordPointer(2,GL_FLOAT,0,0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexPointer(3,GL_FLOAT,0,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        mat4x4_perspective(p, 45, 2, 0.1, 100);

	/* draw marty */
	mat4x4_translate(m,0,-4,-14);
	mat4x4_rotate_Y(m,m,(float)glfwGetTime());
        mat4x4_mul(mvp, p, m);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
	   //#
	   glUniformMatrix4fv(m_location, 1, GL_FALSE, (const GLfloat*) m);
	   glUniform3fv(vpos_location, 1,&cam[0]);
	   glUniform3fv(lightPos_location, 1,&LIGHTPOS[0]);
	   glUniform1f(isMarty_location, (GLfloat)1.1);
    	glUniform1i(glGetUniformLocation(program, "texturemap"), 0);
    	glBindTexture(GL_TEXTURE_2D,martyTextureMap);
    	glActiveTexture(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, numTriangles[0]*3, GL_UNSIGNED_INT,0);

	//#
	// glDisable(GL_TEXTURE_2D);
	// glDisable(GL_TEXTURE0);
	/* draw rock 1 */
	mat4x4_identity(m);
	mat4x4_translate_in_place(m,0,-4,-14);
	mat4x4_rotate_Y(m,m,(float)glfwGetTime());
	mat4x4_scale_aniso(m,m,0.4,0.4,0.4);
        mat4x4_mul(mvp, p, m);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
	   //#
	   glUniformMatrix4fv(m_location, 1, GL_FALSE, (const GLfloat*) m);
	   glUniform3fv(vpos_location, 1,&cam[0]);
	   glUniform3fv(lightPos_location, 1,&LIGHTPOS[0]);
	   glUniform1f(isMarty_location, (GLfloat)0.0);
    	glUniform1i(glGetUniformLocation(program, "texturemap"), 0);
    	glBindTexture(GL_TEXTURE_2D,rockTextureMap);
    	glActiveTexture(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, numTriangles[1]*3, GL_UNSIGNED_INT,(GLvoid *)(sizeof(GLuint)*numTriangles[0]*3));

	/* draw rock 2 */
	mat4x4_identity(m);
	mat4x4_translate_in_place(m,0,-4,-14);
	mat4x4_rotate_Y(m,m,(float)glfwGetTime());
	mat4x4_translate_in_place(m,3,0,0);
        mat4x4_mul(mvp, p, m);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
	   //#
	   glUniformMatrix4fv(m_location, 1, GL_FALSE, (const GLfloat*) m);
	   glUniform3fv(vpos_location, 1,&cam[0]);
	   glUniform3fv(lightPos_location, 1,&LIGHTPOS[0]);
	   glUniform1f(isMarty_location, (GLfloat)0.0);
    	glUniform1i(glGetUniformLocation(program, "texturemap"), 0);
    	glBindTexture(GL_TEXTURE_2D,moldTextureMap);
    	glActiveTexture(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, numTriangles[2]*3, GL_UNSIGNED_INT,(GLvoid *)(sizeof(GLuint)*(numTriangles[0]+numTriangles[1])*3));

	while (glfwGetTime() < lastTime + 1.0/TARGET_FPS)
	{
	}

	lastTime += 1.0/TARGET_FPS;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

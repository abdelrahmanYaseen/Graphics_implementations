#version 120

varying vec2 texture_coordinate;
varying vec3 normal;
varying vec4 pos;
varying vec4 fragWorldPos;
varying vec3 fragWorldNor;
uniform mat4 M;
uniform mat4 MVP;



void main()
{
     gl_Position = MVP*gl_Vertex;
     normal = gl_Normal;
     texture_coordinate.y = 1-gl_MultiTexCoord0.y;
     texture_coordinate.x = gl_MultiTexCoord0.x;
     fragWorldNor = transpose(mat3x3(M)) * gl_Normal;
     fragWorldPos = M*gl_Vertex;


}

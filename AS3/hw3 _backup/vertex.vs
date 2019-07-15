#version 120

uniform mat4 MVP;
uniform mat4 M;

varying vec2 texture_coordinate;
varying vec3 normal;
varying vec4 pos;


//code

varying vec4 fragWorldPos;
varying vec3 fragWorldNor;
//end

mat3 inverse(mat3 m) {
  float d = 1.0 / determinant(m) ;
  return d * mat3( m[2][2]*m[1][1] - m[1][2]*m[2][1],
                    m[1][2]*m[2][0] - m[2][2]*m[1][0],
                     m[2][1]*m[1][0] - m[1][1]*m[2][0] ,

                   m[0][2]*m[2][1] - m[2][2]*m[0][1],
                    m[2][2]*m[0][0] - m[0][2]*m[2][0],
                     m[0][1]*m[2][0] - m[2][1]*m[0][0],

                   m[1][2]*m[0][1] - m[0][2]*m[1][1],
                    m[0][2]*m[1][0] - m[1][2]*m[0][0],
                     m[1][1]*m[0][0] - m[0][1]*m[1][0]
                 );
}


void main()
{
	gl_Position = MVP*gl_Vertex;

	normal = gl_Normal;

	texture_coordinate.x = gl_MultiTexCoord0.x;
	texture_coordinate.y = 1-gl_MultiTexCoord0.y;

	 fragWorldPos = M*gl_Vertex;
	 fragWorldNor = inverse(transpose(mat3x3(M))) * gl_Normal;


}

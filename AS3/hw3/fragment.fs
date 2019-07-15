#version 120

varying vec2 texture_coordinate;
varying vec4 pos;
varying vec3 normal;

uniform sampler2D texturemap;
vec3 I = vec3(1,1,1);
vec3 Iamb = vec3(0.8,0.8,0.8);
// vec3 kd = vec3(1,0.2,0.2);
vec3 kd = vec3(1.0,0.1,0.1);
// vec3 kd = vec3(EYEPOSTITION[0]/10,EYEPOSTITION[0]/10,EYEPOSTITION[0]/10);
// vec3 kd = normalize(EYEPOSTITION);


vec3 ka = vec3(0.3, 0.3, 0.3);
vec3 ks = vec3(0.8, 0.8, 0.8);
uniform vec3 LIGHTPOS;

uniform vec3 EYEPOSTITION;
varying vec4 fragWorldPos;
varying vec3 fragWorldNor;

uniform float ISROCK;


void renderMarty(){
     vec4 cx = texture2D(texturemap, texture_coordinate);
     gl_FragColor = cx;
}
void renderRock(){
     vec3 L = normalize(LIGHTPOS - vec3(fragWorldPos));
     vec3 V = normalize(EYEPOSTITION - vec3(fragWorldPos));
     vec3 H = normalize(L + V);
     vec3 N = normalize(fragWorldNor);
     float NdotL = dot(N, L);
     float NdotH = dot(N, H);
     vec3 diffuseColor = I * kd * max(0, NdotL);
     vec3 specularColor = I * ks * pow(max(0, NdotH), 100);
     vec3 ambientColor = Iamb * ka;

     gl_FragColor = vec4(diffuseColor + specularColor + ambientColor, 1);
}
void main()
{

      if(ISROCK < 0){
           renderMarty();
      } else {
           renderRock();
      }
}

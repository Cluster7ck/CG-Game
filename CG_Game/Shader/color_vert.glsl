uniform float MaxHeight;
uniform float MinHeight;

varying vec3 Normal;
varying vec3 Position;
varying vec2 Texcoord;
varying vec3 PointWorldPos;
varying vec4 VertexColor;

float map(float s, float a1, float a2, float b1, float b2)
{
    return b1 + (s-a1)*(b2-b1)/(a2-a1);
}
void main(void) {
	Position = (gl_ModelViewMatrix * gl_Vertex).xyz;
	PointWorldPos =  gl_Vertex.xyz;
	Normal = gl_NormalMatrix * gl_Normal;
	Texcoord = gl_MultiTexCoord0.xy;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	float HeightRemapped = map(PointWorldPos.y,MinHeight,MaxHeight,0,1);
	if(HeightRemapped < 0.33f ){
		VertexColor = vec4(1.0,1.0,0,0);
	}
	else {
			if(HeightRemapped < 0.66f){
				VertexColor = vec4(0,1.0,1.0,0);
			}
			else{
				VertexColor = vec4(1.0,0,1.0,0);
			}   
	}
}
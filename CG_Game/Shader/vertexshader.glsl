varying vec3 Normal;
varying vec3 Position;
varying vec2 Texcoord;
varying vec3 PointWorldPos;

void main(void) {
	Position = (gl_ModelViewMatrix * gl_Vertex).xyz;
	if(gl_Vertex.z == 0)
		PointWorldPos =  vec3(1.0,1.0,1.0);
	else
		PointWorldPos =  gl_Vertex.xyz;
	Normal = gl_NormalMatrix * gl_Normal;
	Texcoord = gl_MultiTexCoord0.xy;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
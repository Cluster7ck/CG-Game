uniform vec3 LightPos;
uniform vec3 LightColor;

uniform vec3 DiffColor;
uniform vec3 SpecColor;
uniform vec3 AmbientColor;
uniform float SpecExp;

uniform float MaxHeight;
uniform float MinHeight;

varying vec3 Normal;
varying vec3 Position;
varying vec2 Texcoord;
varying vec3 PointWorldPos;

float sat(float a) {
    return clamp(a, 0.0, 1.0);
}
float map(float s, float a1, float a2, float b1, float b2)
{
    return b1 + (s-a1)*(b2-b1)/(a2-a1);
}

void main(void) {
    vec3 N = normalize(Normal);
    vec3 L = normalize(LightPos - Position);
    vec3 E = normalize(- Position);
    vec3 R = reflect(- L, N);

    vec3 DiffuseComponent = DiffColor * sat(dot(N, L));
    

    vec3 SpecularComponent = SpecColor * pow(sat(dot(E, R)), SpecExp);

    //vec3 H = 0.5*(L + E);
    //vec3 SpecularComponent = SpecColor * pow(sat(dot(H, N)), SpecExp);

	//float HeightRemapped = (WorldPos.z - MinHeight) * 1/ (MaxHeight-MinHeight);
	float HeightRemapped = map(PointWorldPos.y,MinHeight,MaxHeight,0,1);
	vec3 DiffuseTexColor;
	if(HeightRemapped < 0.33f ){
		DiffuseTexColor = vec4(1.0,1.0,0,0);
	}
	else {
			if(HeightRemapped < 0.66f){
				DiffuseTexColor = vec4(0,1.0,1.0,0);
			}
			else{
				DiffuseTexColor = vec4(1.0,0,1.0,0);
			}   
	}
 
	
    DiffuseComponent *= DiffuseTexColor;
    vec3 AmbientComponent = AmbientColor * DiffuseTexColor;

    gl_FragColor = vec4(DiffuseComponent + SpecularComponent + AmbientComponent, 0);
	//gl_FragColor = vec4(1.0,1.0,0,0);
}
#version  130
uniform vec3 LightPos;
uniform vec3 LightColor;

uniform vec3 DiffColor;
uniform vec3 SpecColor;
uniform vec3 AmbientColor;
uniform float SpecExp;

uniform float MaxHeight;
uniform float MinHeight;

in vec3 Normal;
in vec3 Position;
in vec2 Texcoord;
in vec3 PointWorldPos;

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
	vec4 DiffuseTexColor;
	vec4 TopColor = vec4(0.2,0.6,0,0);
	vec4 MidColor = vec4(1.0,0,1.0,0);
	vec4 BotColor =  vec4(0.6,0.8,0,0);
	DiffuseTexColor = mix (MidColor, TopColor, HeightRemapped);
	DiffuseTexColor = mix (BotColor, DiffuseTexColor, HeightRemapped);
	
	//DiffuseTexColor = mix(BotColor, MidColor, HeightRemapped);
	//DiffuseTexColor = mix(DiffuseTexColor, TopColor, HeightRemapped);
	/*
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
	*/
    DiffuseComponent *= vec3(DiffuseTexColor);
    vec3 AmbientComponent = AmbientColor * vec3(DiffuseTexColor);
    gl_FragColor = vec4(DiffuseComponent + SpecularComponent + AmbientComponent, 0);
	//vec4(DiffuseTexColor,0);//
	//gl_FragColor = vec4(1.0,1.0,0,0);
}
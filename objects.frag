#version 450

struct Light { 
	vec4 Tint; //StrPow
	vec4 Pos;  //AngFov
	vec4 RLB;  //Type
	vec4 Dir;
};

layout(set=0, binding=0, std140) readonly buffer World {
	Light Lights[];
};

layout(set=2, binding=0) uniform sampler2D TEXTURE;
layout(set=3, binding=0) uniform sampler2D ENVIRONMENT;

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texCoord;
layout(location=3) in flat uint matType;

layout(location=0) out vec4 outColor;

float PI = 3.14159265; 


void main() {
    vec3 n = normalize(normal);
	//vec3 l = vec3(0.0, 0.0, 1.0);
	vec3 albedo = texture(TEXTURE, texCoord).rgb;
	vec2 envCoord = vec2((n.x + 1)/2 , (n.y + 1) /2 );
	vec3 env = texture(ENVIRONMENT, envCoord).rgb;

	//radience lighting
	vec3 e = vec3(0.0, 0.0, 0.0);
	int size = Lights.length();
	for(int i = 0; i < size; i++){
		Light light = Lights[i];
		//extract all
		vec3 tint = light.Tint.rgb;
		float strPow = light.Tint.w;
		vec3 pos = light.Pos.rgb;
		float angFov = light.Pos.a;
		float radius = light.RLB.x;
		float limit = light.RLB.y;
		float blend = light.RLB.z;
		float type = light.RLB.w;
		vec3 dir = light.Dir.xyz;

		vec3 l = pos - position;
		float d = length(l);
		l = normalize(l);
		float contribution = dot(n, l);
		vec3 energy = tint;
		float attenuation = 1.f;
		if(type == 0.f){ //Sun
			l = pos;
			//l = normalize(l);
			contribution = dot(n, l);
			float sin2 = sin(angFov/2.0);
			if(contribution < -sin2){
				contribution = 0.0;
			} else if(contribution < sin2){
				float a = contribution + sin2;
				float b = 2.0 * sin2;
				contribution = pow((a / b), 2.0); 
			}
			energy = tint * strPow;
			//energy = vec3(1.0,0.0,.0);
		} else if(type < 2.f){ //Sphere
			//contribution  = dot(n, l);
			float sin2 = radius/d;
			if(contribution < -sin2){
				contribution = 0.0;
			} else if(contribution < sin2){
				float a = contribution + sin2;
				float b = 2.0 * sin2;
				contribution = pow((a / b), 2.0); 
			}
			attenuation = max(0.0, 1.0 - pow(d/limit, 4));
			energy = (tint * strPow) /( 4.0 * PI * max(d,radius));
			//energy = vec3(0.0,1.0,0.0);
		} else if(type < 3.f){ //Spot
			float sin2 = radius/d;
			if(contribution < -sin2){
				contribution = 0.0;
			} else if(contribution < sin2){
				float a = contribution + sin2;
				float b = 2.0 * sin2;
				contribution = pow((a / b), 2.0); 
			}

			//Check if it's in the cone
			float theta = acos(dot(dir,l));
			float blendpoint = (angFov * (1.0 - blend)) / 2.0;
			if(theta > angFov / 2.0){
				contribution = 0.0;
			} else if (theta > blendpoint){
				//blend the light
				float a = theta - blendpoint;
				float b = (angFov / 2.0) - theta;
				contribution *= 1 - (a / (a + b));
			}
			attenuation = max(0.0, 1.0 - pow(d/limit, 4));
			energy = (tint * strPow) /( 4.0 * PI * max(d,radius));
			//energy = vec3(0.0,0.0,1.0);
		} 

		e = e + energy * contribution * attenuation;	
	}

	vec3 toned =  {1.0 - (1.0/(exp(e.x))), 1.0 - (1.0/(exp(e.y))), 1.0 - (1.0/(exp(e.z))) };

	outColor = vec4(toned * albedo, 1.0);
}
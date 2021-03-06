#version 420 core

uniform vec4 		objectColour;
layout (binding = 0) uniform sampler2D mainTex;
layout (binding = 2) uniform sampler2D paintMaskTex;
layout (binding = 1) uniform sampler2DShadow shadowTex;
layout (binding = 3) uniform sampler2D normalTex;
layout (binding = 4) uniform sampler2D noiseTex;

uniform vec3	lightPos;
uniform float	lightRadius;
uniform vec4	lightColour;

uniform vec3	cameraPos;

uniform bool hasTexture;
uniform bool hasPaintMask;
uniform bool hasNormal;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void)
{
	float shadow = 1.0;
	vec3 useNormal = IN.normal;

	if( IN . shadowProj . w > 0.0) {
		shadow = textureProj ( shadowTex , IN . shadowProj ) * 0.5f;
	}

	vec3  incident = normalize ( lightPos - IN.worldPos );
	vec3 viewDir = normalize ( cameraPos - IN . worldPos );
	vec3 halfDir = normalize ( incident + viewDir );

	vec4 albedo = IN.colour;

	if (albedo.a < 0.01){
		discard;
	}
	
	if(hasTexture) {
		albedo *= texture(mainTex, IN.texCoord);
	}

	mat3 TBN = mat3(normalize(IN.tangent),
    normalize(IN.binormal), normalize(IN.normal ));

	if (hasNormal){
    	vec3 bumpNormal = texture(normalTex , IN.texCoord ).rgb;
    	bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));
		useNormal = bumpNormal;
	}

	if(hasPaintMask) {
		vec4 col = texture(paintMaskTex, IN.texCoord).rgba;
		albedo.rgb = (albedo.rgb * (1-col.a)) + (vec3(col.r, col.g, col.b) * col.a);
		if (col.a > 0.1){
			vec3 bumpNormal = texture(noiseTex , IN.texCoord ).rgb;
    		bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));
			useNormal = bumpNormal;
		}
	}

	
	float lambert  = max (0.0 , dot ( incident , useNormal )) * 0.9; 
	float rFactor = max (0.0 , dot ( halfDir , useNormal ));
	float sFactor = pow ( rFactor , 80.0 );


	
	albedo.rgb = pow(albedo.rgb, vec3(2.2));
	
	fragColor.rgb = albedo.rgb * 0.05f; //ambient
	
	fragColor.rgb += albedo.rgb * lightColour.rgb * lambert * shadow; //diffuse light
	
	fragColor.rgb += lightColour.rgb * sFactor * shadow; //specular light
	
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2f));
	
	fragColor.a = albedo.a;
}
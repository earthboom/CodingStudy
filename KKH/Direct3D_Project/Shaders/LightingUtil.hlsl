
#define MaxLights 16

struct Light
{
	float3 Strength;
	float FalloffStart;		//point / spot light only
	float3 Direction;		//directional / spot light only
	float FalloffEnd;		//point / spot light only		
	float3 Position;		//point light only
	float SpotPower;		//spo light only
};

struct Material
{
	float4 DiffuseAlbedo;
	float3 FresnelR0;
	float Shininess;
};



float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
	return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

// R0 = ( (n-1) / (n+1) )^2
float3 SclickFresnel(float R0, float3 normal, float lightVec)
{
	float cosIncidentAngle = saturate(dot(normal, lightVec));

	float f0 = 1.0f - cosIncidentAngle;
	float3 reflectParcent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0);

	return reflectParcent;
}

float3 BlinnPhong(float lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
	const float m = mat.Shininess * 256.0f;
	float3 halfVec = normalize(toEye + lightVec);

	float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
	float3 fresnelFactor = SclickFresnel(mat.FresnelR0, halfVec, lightVec);

	float3 specAlbedo = fresnelFactor * roughnessFactor;

	specAlbedo = specAlbedo / (specAlbedo + 1.0f);

	return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStrength;
}

float3 ComputeDirectionalLight(Light L, Material mat, float3 normal, float3 toEye)
{
	float3 lightVec = -L.Direction;

	float ndotl = max(dot(lightVec, normal), 0.0f);
	float3 lightStrength = L.Strength * ndotl;

	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
	float3 lightVec = L.Position - pos;

	float d = length(lightVec);

	if (d > L.FalloffEnd)
		return 0.0f;

	lightVec /= d;

	float ndotl = mat(dot(lightVec, normal), 0.0f);
	float3 lightStrength = L.Strength * ndotl;

	float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
	lightStrength *= att;

	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
	float3 lightVec = L.Position - pos;

	float d = length(lightVec);

	if (d > L.FalloffEnd)
		return 0.0f;

	lightVec /= d;

	float ndotl = mat(dot(lightVec, normal), 0.0f);
	float3 lightStrength = L.Strength * ndotl;

	float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
	lightStrength *= att;

	float spotFactor = pow(max(dot(-lightVec, L.Direction), 0.0f), L.SpotPower);
	lightStrength *= spotFactor;

	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

float4 ComputeLighting(Light gLoghts[MaxLights], Material mat, 
	float pos, float3 normal, float3 toEye, float3 shadowFactor)
{
	float3 result = 0.0f;

#if (NUM_DIR_LIGHTS > 0)
	for (int i = 0; i < NUM_DIR_LIGHTS; ++i)
	{
		result += shadowFactor[i] * ComputeDirectionalLight(gLight[i], mat, normal, toEye);
	}
#endif

#if (NUM_POINT_LIGHTS > 0)
	for (int i = NUM_DIR_LIGHTS; o < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
	{
		result += ComputePointLight(gLights[i], mat, pos, normal, toEye);
	}
#endif

#if (NUM_SPOT_LIGHTS > 0)
	for (int i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; o < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
	{
		result += ComputeSpotLight(gLights[i], mat, pos, normal, toEye);
	}
#endif

	return float4(result, 0.0f);
}


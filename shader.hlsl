cbuffer ConstantBuffer
{
	matrix	worldViewProjection;
	matrix  worldTransformation;
	float4	ambientLightColour;
	float4  directionalLightColour;
	float4  directionalLightVector;
	float3	eyePosition;
	float	SpecularPower;
	float4	SpecularColour;
};

struct VertexIn
{
	float3 InputPosition : POSITION;
	float3 Normal		 : NORMAL;
};

struct VertexOut
{
	float4 OutputPosition	: SV_POSITION;
	float4 Normal			: TEXCOORD0;
	float4 WorldPosition	: TEXCOORD1;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.OutputPosition = mul(worldViewProjection, float4(vin.InputPosition, 1.0f));

	float4 adjustedNormal = normalize(mul(worldTransformation, float4(vin.Normal, 0.0f)));

	vout.Normal = adjustedNormal;
	vout.WorldPosition = mul(worldTransformation, float4(vin.InputPosition, 1.0f));

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	//Get vector back to the light source
	float4 vectorBackToLight = -directionalLightVector;
	
	//Find the light direction, and view direction
	float4 lightDir = normalize(pin.WorldPosition - vectorBackToLight);
	float4 viewDir = normalize(pin.WorldPosition - float4(eyePosition, 1.0f));

	//Finding the halfway direction and magnitude.
	float4 halfwayDir = normalize(lightDir - viewDir);
	float halfwayMag = length(halfwayDir);

	//Normalizing and dividing to find halfway point.
	float4 halfway = normalize(halfwayDir) / normalize(halfwayMag);

	//calculating the specular light
	float specular = pow(saturate(dot(pin.Normal, halfway)), SpecularPower);

	//multiplying specular light by the colour to apply its colour
	float4 specCol = specular * SpecularColour;

	// calculate the diffuse light and add it to the ambient light
	float diffuseBrightness = saturate(dot(pin.Normal, vectorBackToLight));

	//Calculating the colour by all the different attributes
	float4 Colour = saturate(ambientLightColour) + saturate(diffuseBrightness) * saturate(directionalLightColour);
	//Adding the spcular to the pixels
	float4 newColour = Colour + specCol;

	return newColour;
}

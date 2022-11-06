cbuffer ConstantBuffer 
{
	matrix worldViewProjection; 
};

struct VertexIn
{
	float4 InputPosition : POSITION;
	float4 Colour : COLOR;
};

struct VertexOut
{
	float4 OutputPosition : SV_POSITION;
	float4 Colour : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.Colour = vin.Colour;
	// Transform to homogeneous clip space.
	vout.OutputPosition = mul(worldViewProjection, vin.InputPosition);
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{	
	return pin.Colour;	
}



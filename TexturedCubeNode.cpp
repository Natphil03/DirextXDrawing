#include "TexturedCubeNode.h"
#include "DirectXFramework.h"
#include "WICTextureLoader.h"
#include <vector>

// DirectX libraries that are needed
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

bool TexturedCubeNode::Initialise()
{
	//Gathering key variables from framework.
	DirectXFramework* _DXFramework = DirectXFramework::GetDXFramework();
	_device = _DXFramework->GetDevice();
	_deviceContext = _DXFramework->GetDeviceContext();
	_viewTransformation = _DXFramework->GetViewTransformation();
	_projectionTransformation = _DXFramework->GetProjectionTransformation();
	_eyePosition = _DXFramework->GetEyePos();

	//Getting common cbuffer values
	_directionalLightColour = _DXFramework->GetDirectionalLightColour();
	_directionalLightVector = _DXFramework->GetDirectionalLightVector();
	_specularColour = _DXFramework->GetSpecularColour();
	_specularPower = _DXFramework->GetSpecularPower();

	calculateNormals();
	BuildGeometryBuffers();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	BuildRasteriserState();
	BuildTexture();

	return true;
}

void TexturedCubeNode::Render()
{
	const float clearColour[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	// Calculate the world x view x projection transformation
	Matrix completeTransformation = _cumulativeWorldTransformation * _viewTransformation * _projectionTransformation;

	//Setting constant buffer fields.
	CBuffer constantBuffer;
	constantBuffer.WorldViewProjection = completeTransformation;
	constantBuffer.World = _cumulativeWorldTransformation;
	constantBuffer.AmbientLightColour = _ambientLightColour;
	constantBuffer.DirectionalLightColour = _directionalLightColour;
	constantBuffer.DirectionalLightVector = _directionalLightVector;
	constantBuffer.EyePosition = _eyePosition;
	constantBuffer.SpecularPower = _specularPower;
	constantBuffer.SpecularColour = _specularColour;

	// Update the constant buffer. Note the layout of the constant buffer must match that in the shader
	_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceContext->PSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	
	_deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &constantBuffer, 0, 0);

	//Sets the texture to be used by pixel shader.
	_deviceContext->PSSetShaderResources(0, 1, _texture.GetAddressOf());

	// Now render the cube
	// Specify the distance between vertices and the starting point in the vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Set the vertex buffer and index buffer we are going to use
	_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Specify the layout of the polygons (it will rarely be different to this)
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Specify the layout of the input vertices.  This must match the layout of the input vertices in the shader
	_deviceContext->IASetInputLayout(_layout.Get());

	// Specify the vertex and pixel shaders we are going to use
	_deviceContext->VSSetShader(_vertexShader.Get(), 0, 0);
	_deviceContext->PSSetShader(_pixelShader.Get(), 0, 0);

	// Specify details about how the object is to be drawn
	_deviceContext->RSSetState(_rasteriserState.Get());

	// Now draw the first cube
	_deviceContext->DrawIndexed(ARRAYSIZE(indices), 0, 0);
}

void TexturedCubeNode::Shutdown()
{
}

void TexturedCubeNode::calculateNormals()
{
	for (int i = 0; i < ARRAYSIZE(indices); i += 3)
	{
		// Get all 3 points for a polygon.
		Vector3 p1 = vertices[indices[i]].Position;
		Vector3 p2 = vertices[indices[i + 1]].Position;
		Vector3 p3 = vertices[indices[i + 2]].Position;
		Vector3 normalPolygon = calculatePolygonNormals(p1, p2, p3);//calculate the polygon normal.

		//for each point we add the normal to the normalized polygon
		vertices[indices[i]].Normal = vertices[indices[i]].Normal + normalPolygon;
		polygonCount[indices[i]]++;
		vertices[indices[i + 1]].Normal = vertices[indices[i + 1]].Normal + normalPolygon;
		polygonCount[indices[i + 1]]++;
		vertices[indices[i + 2]].Normal = vertices[indices[i + 2]].Normal + normalPolygon;
		polygonCount[indices[i + 2]]++;

	}

	for (int i = 0; i < ARRAYSIZE(vertices); i++)
	{
		(vertices[i].Normal /= polygonCount[i]).Normalize(); // normalize after calculating the normal divided by polygon count for the vertice.
	}
}

Vector3 TexturedCubeNode::calculatePolygonNormals(Vector3 p1, Vector3 p2, Vector3 p3)
{
	Vector3 normalPolygon;

	Vector3 v1v2 = p1 - p2;
	Vector3 v1v3 = p1 - p3;

	normalPolygon = v1v2.Cross(v1v3);

	return normalPolygon;
}


void TexturedCubeNode::BuildGeometryBuffers()
{
	// This method uses the arrays defined in Geometry.h
	// 
	// Setup the structure that specifies how big the vertex 
	// buffer should be
	D3D11_BUFFER_DESC vertexBufferDescriptor = { 0 };
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
	vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescriptor.CPUAccessFlags = 0;
	vertexBufferDescriptor.MiscFlags = 0;
	vertexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the vertices from
	D3D11_SUBRESOURCE_DATA vertexInitialisationData = { 0 };
	vertexInitialisationData.pSysMem = &vertices;

	// and create the vertex buffer
	ThrowIfFailed(_device->CreateBuffer(&vertexBufferDescriptor, &vertexInitialisationData, _vertexBuffer.GetAddressOf()));

	// Setup the structure that specifies how big the index 
	// buffer should be
	D3D11_BUFFER_DESC indexBufferDescriptor = { 0 };
	indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.ByteWidth = sizeof(UINT) * ARRAYSIZE(indices);
	indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescriptor.CPUAccessFlags = 0;
	indexBufferDescriptor.MiscFlags = 0;
	indexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the indices from
	D3D11_SUBRESOURCE_DATA indexInitialisationData{};
	indexInitialisationData.pSysMem = &indices;

	// and create the index buffer
	ThrowIfFailed(_device->CreateBuffer(&indexBufferDescriptor, &indexInitialisationData, _indexBuffer.GetAddressOf()));
}

void TexturedCubeNode::BuildShaders()
{
	DWORD shaderCompileFlags = 0;
#if defined( _DEBUG )
	shaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> compilationMessages = nullptr;

	//Compile vertex shader
	HRESULT hr = D3DCompileFromFile(ShaderFileName,
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		VertexShaderName, "vs_5_0",
		shaderCompileFlags, 0,
		_vertexShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	// Even if there are no compiler messages, check to make sure there were no other errors.
	ThrowIfFailed(hr);
	ThrowIfFailed(_device->CreateVertexShader(_vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), NULL, _vertexShader.GetAddressOf()));

	// Compile pixel shader
	hr = D3DCompileFromFile(ShaderFileName,
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		PixelShaderName, "ps_5_0",
		shaderCompileFlags, 0,
		_pixelShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	ThrowIfFailed(hr);
	ThrowIfFailed(_device->CreatePixelShader(_pixelShaderByteCode->GetBufferPointer(), _pixelShaderByteCode->GetBufferSize(), NULL, _pixelShader.GetAddressOf()));
}

void TexturedCubeNode::BuildVertexLayout()
{
	// Create the vertex input layout. This tells DirectX the format
	// of each of the vertices we are sending to it. The vertexDesc array is
	// defined in Geometry.h

	ThrowIfFailed(_device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), _vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), _layout.GetAddressOf()));
}

void TexturedCubeNode::BuildConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ThrowIfFailed(_device->CreateBuffer(&bufferDesc, NULL, _constantBuffer.GetAddressOf()));

}

void TexturedCubeNode::BuildRasteriserState()
{
	D3D11_RASTERIZER_DESC rasteriserDesc{};
	rasteriserDesc.CullMode = D3D11_CULL_BACK;
	rasteriserDesc.FrontCounterClockwise = false;
	rasteriserDesc.DepthBias = 0;
	rasteriserDesc.SlopeScaledDepthBias = 0.0f;
	rasteriserDesc.DepthBiasClamp = 0.0f;
	rasteriserDesc.DepthClipEnable = true;
	rasteriserDesc.ScissorEnable = false;
	rasteriserDesc.MultisampleEnable = false;
	rasteriserDesc.AntialiasedLineEnable = false;
	rasteriserDesc.FillMode = D3D11_FILL_SOLID;
	ThrowIfFailed(_device->CreateRasterizerState(&rasteriserDesc, _rasteriserState.GetAddressOf()));
}

void TexturedCubeNode::BuildTexture()
{
	ThrowIfFailed(CreateWICTextureFromFile(_device.Get(), 
		_deviceContext.Get(), 
		boxTextureName, 
		nullptr, 
		_texture.GetAddressOf()));

}

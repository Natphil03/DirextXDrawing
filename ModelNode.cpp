#include "ModelNode.h"
#include "DirectXFramework.h"

bool ModelNode::Initialise()
{
	//Gathering key variables from framework
	DirectXFramework* _DXFramework = DirectXFramework::GetDXFramework();
	_device = _DXFramework->GetDevice();
	_deviceContext = _DXFramework->GetDeviceContext();
	_viewTransformation = _DXFramework->GetViewTransformation();
	_projectionTransformation = _DXFramework->GetProjectionTransformation();
	_eyePosition = _DXFramework->GetEyePos();

	//Getting resource manager and mesh for model.
	_resourceManager = _DXFramework->GetResourceManager();
	_mesh = _resourceManager->GetMesh(L"airplane.x");
	
	//Getting common cbuffer values
	_directionalLightColour = _DXFramework->GetDirectionalLightColour();
	_directionalLightVector = _DXFramework->GetDirectionalLightVector();
	_specularColour = _DXFramework->GetSpecularColour();
	_specularPower = _DXFramework->GetSpecularPower();

	BuildShaders();
	BuildRasteriserState();
	BuildVertexLayout();
	BuildConstantBuffer();

	return true;
}

void ModelNode::Render()
{
	const float clearColour[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	// Calculate the world x view x projection transformation
	Matrix completeTransformation = _cumulativeWorldTransformation * _viewTransformation * _projectionTransformation;

	//These are the getters for the relevant variables in which we can load in for each submesh, this current mesh does not have good lighting 
	// so I have added my own. This is to show you I can gather these values.
	//_subMesh->GetMaterial()->GetDiffuseColour();
	//_subMesh->GetMaterial()->GetShininess();
	//_subMesh->GetMaterial()->GetSpecularColour();

	CBuffer constantBuffer;
	constantBuffer.WorldViewProjection = completeTransformation;
	constantBuffer.World = _cumulativeWorldTransformation;
	constantBuffer.AmbientLightColour = _ambientLightColour;
	constantBuffer.DirectionalLightVector = _directionalLightVector;
	constantBuffer.EyePosition = _eyePosition;
	constantBuffer.DirectionalLightColour = _directionalLightColour;
	constantBuffer.SpecularPower = _specularPower;
	constantBuffer.SpecularColour = _specularColour;

	_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceContext->PSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &constantBuffer, 0, 0);

	for (int i = 0; i < _mesh->GetSubMeshCount(); i++)
	{
		_subMesh = _mesh->GetSubMesh(i);

		// Now render the cube
		// Specify the distance between vertices and the starting point in the vertex buffer
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		_deviceContext->IASetVertexBuffers(0, 1, _subMesh->GetVertexBuffer().GetAddressOf(), &stride, &offset);
		_deviceContext->IASetIndexBuffer(_subMesh->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//// Specify the layout of the input vertices.  This must match the layout of the input vertices in the shader
		_deviceContext->IASetInputLayout(_layout.Get());
		_deviceContext->VSSetShader(_vertexShader.Get(), 0, 0);

		//If has texture coordinates then apply texture and use texture pixel shader. otherwise use normal pixelshader.
		if (_subMesh->HasTexCoords())
		{
			_deviceContext->PSSetShader(_texturePixelShader.Get(), 0, 0);
			_texture = _subMesh->GetMaterial()->GetTexture();//Get texture from submesh.
			_deviceContext->PSSetShaderResources(0, 1, _texture.GetAddressOf()); //apply texture to PS 
		}
		else
		{
			_deviceContext->PSSetShader(_pixelShader.Get(), 0, 0);
		}

		//Now draw the first cube
		_deviceContext->DrawIndexed(_subMesh->GetIndexCount(), 0, 0);
	}
}

void ModelNode::Shutdown()
{
	_resourceManager->ReleaseMesh(L"airplane.x");
}

void ModelNode::BuildShaders()
{
	DWORD shaderCompileFlags = 0;
#if defined( _DEBUG )
	shaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> compilationMessages = nullptr;

	//Compile vertex shader
	HRESULT hr = D3DCompileFromFile(textureShaderFileName,
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
	hr = D3DCompileFromFile(textureShaderFileName,
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

	// Compile pixel shader
	hr = D3DCompileFromFile(textureShaderFileName,
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		texturePixelShaderName, "ps_5_0",
		shaderCompileFlags, 0,
		_texturePixelShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	ThrowIfFailed(hr);
	ThrowIfFailed(_device->CreatePixelShader(_texturePixelShaderByteCode->GetBufferPointer(), _texturePixelShaderByteCode->GetBufferSize(), NULL, _texturePixelShader.GetAddressOf()));
}


void ModelNode::BuildVertexLayout()
{
	// Create the vertex input layout. This tells DirectX the format
	// of each of the vertices we are sending to it. The vertexDesc array is
	// defined in Geometry.h

	ThrowIfFailed(_device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), _vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), _layout.GetAddressOf()));
}

void ModelNode::BuildConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ThrowIfFailed(_device->CreateBuffer(&bufferDesc, NULL, _constantBuffer.GetAddressOf()));
}

void ModelNode::BuildRasteriserState()
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
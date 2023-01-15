#pragma once
#include "SceneNode.h"
#include "Mesh.h"
#include "ResourceManager.h"
#include <vector>

#define textureShaderFileName		L"ModelShader.hlsl"
//#define ShaderFileName				L"shader.hlsl"
#define VertexShaderName	"VS"
#define PixelShaderName		"PS"
#define texturePixelShaderName		"TPS"

class ModelNode : public SceneNode
{

public:
	ModelNode(wstring name, Vector4 ambientLightColour) : SceneNode(name) { _ambientLightColour = ambientLightColour; };
	~ModelNode(void) {};

	bool Initialise() override;
	void Render() override;
	void Shutdown() override;

private:
	Vector4 _ambientLightColour;

	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	void BuildRasteriserState();

	shared_ptr<ResourceManager> _resourceManager;

	shared_ptr<Mesh> _mesh;
	shared_ptr<SubMesh> _subMesh;

	ComPtr<ID3D11Device>			_device;
	ComPtr<ID3D11DeviceContext>		_deviceContext;
	ComPtr<IDXGISwapChain>			_swapChain;
	ComPtr<ID3D11Texture2D>			_depthStencilBuffer;
	ComPtr<ID3D11RenderTargetView>	_renderTargetView;
	ComPtr<ID3D11DepthStencilView>	_depthStencilView;

	ComPtr<ID3D11Buffer>			_vertexBuffer;
	ComPtr<ID3D11Buffer>			_indexBuffer;

	ComPtr<ID3DBlob>				_vertexShaderByteCode = nullptr;
	ComPtr<ID3DBlob>				_pixelShaderByteCode = nullptr;

	ComPtr<ID3DBlob>				_textureVertexShaderByteCode = nullptr;
	ComPtr<ID3DBlob>				_texturePixelShaderByteCode = nullptr;

	ComPtr<ID3D11VertexShader>		_vertexShader;
	ComPtr<ID3D11VertexShader>		_textureVertexShader;
	ComPtr<ID3D11PixelShader>		_pixelShader;
	ComPtr<ID3D11PixelShader>		_texturePixelShader;

	ComPtr<ID3D11InputLayout>		_layout;
	ComPtr<ID3D11Buffer>			_constantBuffer;

	ComPtr<ID3D11RasterizerState>   _rasteriserState;

	Vector4							_directionalLightColour;
	Vector4							_directionalLightVector;
	float							_specularPower{ 0 };
	Vector4							_specularColour;

	Vector3							_eyePosition;
	Vector3							_focalPointPosition;
	Vector3							_upVector;

	Matrix							_viewTransformation;
	Matrix							_projectionTransformation;

	ComPtr<ID3D11ShaderResourceView> _texture;

	struct  CBuffer
	{
		Matrix		WorldViewProjection;
		Matrix		World;
		Vector4		AmbientLightColour;
		Vector4		DirectionalLightColour;
		Vector4		DirectionalLightVector;
		Vector3		EyePosition;
		float		SpecularPower{ 0 };
		Vector4		SpecularColour;
	};

	struct Vertex
	{
		Vector3		Position;
		Vector3		Normal;
		Vector2		TextureCoordinates;
	};

	D3D11_INPUT_ELEMENT_DESC vertexDesc[3] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
};


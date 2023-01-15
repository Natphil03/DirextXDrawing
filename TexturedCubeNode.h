#pragma once
#include "SceneNode.h"

#define ShaderFileName		L"shaderTexture.hlsl"
#define VertexShaderName	"VS"
#define PixelShaderName		"PS"
#define boxTextureName			L"woodbox.bmp"

class TexturedCubeNode : public SceneNode
{

public:
	TexturedCubeNode(wstring name, Vector4 ambientLightColour) : SceneNode(name) { _ambientLightColour = ambientLightColour; };
	~TexturedCubeNode(void) {};

	bool Initialise() override;
	void Render() override;
	void Shutdown() override;

	void calculateNormals();
	Vector3 calculatePolygonNormals(Vector3 p1, Vector3 p2, Vector3 p3);
	
private:
	Vector4				_ambientLightColour;

	void BuildGeometryBuffers();
	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	void BuildRasteriserState();
	void BuildTexture();

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
	ComPtr<ID3D11VertexShader>		_vertexShader;
	ComPtr<ID3D11PixelShader>		_pixelShader;
	ComPtr<ID3D11InputLayout>		_layout;
	ComPtr<ID3D11Buffer>			_constantBuffer;

	ComPtr<ID3D11RasterizerState>   _rasteriserState;

	Vector3							_eyePosition;
	Vector3							_focalPointPosition;
	Vector3							_upVector;

	Vector4							_directionalLightColour;
	Vector4							_directionalLightVector;
	float							_specularPower;
	Vector4							_specularColour;

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
		Vector3 	Normal;
		Vector2		TextureCoordinates;
	};

	D3D11_INPUT_ELEMENT_DESC vertexDesc[3] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	Vertex vertices[24] = 
	{
		{ Vector3(-1.0f, -1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f)  },    // side 1
		{ Vector3(1.0f, -1.0f, 1.0f),  Vector3(0.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f)  },
		{ Vector3(-1.0f, 1.0f, 1.0f),  Vector3(0.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f)	},
		{ Vector3(1.0f, 1.0f, 1.0f),   Vector3(0.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f)	},

		{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f) },    // side 2
		{ Vector3(-1.0f, 1.0f, -1.0f),  Vector3(0.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f)	},
		{ Vector3(1.0f, -1.0f, -1.0f),  Vector3(0.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f) },
		{ Vector3(1.0f, 1.0f, -1.0f),   Vector3(0.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f) },

		{ Vector3(-1.0f, 1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f)  },    // side 3
		{ Vector3(-1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f)  },
		{ Vector3(1.0f, 1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f)  },
		{ Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f) },

		{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f)  },    // side 4
		{ Vector3(1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f) },
		{ Vector3(-1.0f, -1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f)  },
		{ Vector3(1.0f, -1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f)  },

		{ Vector3(1.0f, -1.0f, -1.0f),Vector3(0.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f)  },    // side 5
		{ Vector3(1.0f, 1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f)  },
		{ Vector3(1.0f, -1.0f, 1.0f),Vector3(0.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f)  },
		{ Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f)  },

		{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f) },    // side 6
		{ Vector3(-1.0f, -1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f)  },
		{ Vector3(-1.0f, 1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f)  },
		{ Vector3(-1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f)  }
	};

	float polygonCount[sizeof(vertices) / sizeof(vertices[0])] = {};
	Vector3 polygonNormals[ARRAYSIZE(vertices)];

	UINT indices[36] = {
				0, 1, 2,       // side 1
				2, 1, 3,
				4, 5, 6,       // side 2
				6, 5, 7,
				8, 9, 10,      // side 3
				10, 9, 11,
				12, 13, 14,    // side 4
				14, 13, 15,
				16, 17, 18,    // side 5
				18, 17, 19,
				20, 21, 22,    // side 6
				22, 21, 23,
	};

protected:
};

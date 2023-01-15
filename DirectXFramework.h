#pragma once
#include <vector>
#include "Framework.h"
#include "DirectXCore.h"
#include "SceneGraph.h"
#include "ResourceManager.h"

class DirectXFramework : public Framework
{
public:
	DirectXFramework();
	DirectXFramework(unsigned int width, unsigned int height);

	virtual void CreateSceneGraph();
	virtual void UpdateSceneGraph();

	bool Initialise();
	void Update();
	void Render();
	void OnResize(WPARAM wParam);
	void Shutdown();

	static DirectXFramework *			GetDXFramework();

	inline SceneGraphPointer			GetSceneGraph() { return _sceneGraph; }
	inline ComPtr<ID3D11Device>			GetDevice() { return _device; }
	inline ComPtr<ID3D11DeviceContext>	GetDeviceContext() { return _deviceContext; }
	inline Vector3						GetEyePos() { return _eyePosition; }
	inline shared_ptr<ResourceManager>	GetResourceManager() { return _resourceManager; }

	inline Vector4						GetDirectionalLightColour() { return _DirectionalLightColour; }
	inline Vector4						GetDirectionalLightVector() { return _DirectionalLightVector; }
	inline float						GetSpecularPower() { return _SpecularPower; }
	inline Vector4						GetSpecularColour() { return _SpecularColour; }

	const Matrix&						GetViewTransformation() const;
	const Matrix&						GetProjectionTransformation() const;

	void								SetBackgroundColour(Vector4 backgroundColour);

private:

	shared_ptr<ResourceManager>			_resourceManager;

	ComPtr<ID3D11Device>				_device;
	ComPtr<ID3D11DeviceContext>			_deviceContext;
	ComPtr<IDXGISwapChain>				_swapChain;
	ComPtr<ID3D11Texture2D>				_depthStencilBuffer;
	ComPtr<ID3D11RenderTargetView>		_renderTargetView;
	ComPtr<ID3D11DepthStencilView>		_depthStencilView;

	D3D11_VIEWPORT						_screenViewport{ 0 };

	Vector3								_eyePosition;
	Vector3								_focalPointPosition;
	Vector3								_upVector;

	Matrix								_viewTransformation;
	Matrix								_projectionTransformation;

	SceneGraphPointer					_sceneGraph;

	float							    _backgroundColour[4];

	Vector4		_DirectionalLightColour;
	Vector4		_DirectionalLightVector;
	float		_SpecularPower{ 0 };
	Vector4		_SpecularColour;





	bool GetDeviceAndSwapChain();
};


#include "DirectXApp.h"
#include "ModelNode.h"
#include "CubeNode.h"
#include "TeapotNode.h"
#include "TexturedCubeNode.h"


DirectXApp app;

void DirectXApp::CreateSceneGraph()
{
	SceneGraphPointer _sceneGraph = GetSceneGraph();

	SceneNodePointer modelNode = SceneNodePointer(new ModelNode(L"ModelNode", Vector4(0.2f, 0.2f, 0.2f, 1.0f)));
	
	_sceneGraph->Add(modelNode);

	SceneNodePointer Teapot = SceneNodePointer(new TeapotNode(L"Teapot", Vector4(0.2f, 0.2f, 0.2f, 1.0f)));
	
	_sceneGraph->Add(Teapot);

	SceneNodePointer Body = SceneNodePointer(new TexturedCubeNode(L"Body", Vector4(0.2f, 0.2f, 0.2f, 1.0f)));
	
	_sceneGraph->Add(Body);
	
	SceneNodePointer LeftLeg = SceneNodePointer(new CubeNode(L"LeftLeg", Vector4(0.2f, 0.2f, 0.2f, 1.0f)));
	
	_sceneGraph->Add(LeftLeg);
	
	SceneNodePointer RightLeg = SceneNodePointer(new CubeNode(L"RightLeg", Vector4(0.2f, 0.2f, 0.2f, 1.0f)));
	
	_sceneGraph->Add(RightLeg);

	SceneNodePointer Head = SceneNodePointer(new CubeNode(L"Head", Vector4(0.2f, 0.2f, 0.2f, 1.0f)));
	
	_sceneGraph->Add(Head);

	SceneNodePointer Nose = SceneNodePointer(new CubeNode(L"Nose", Vector4(0.2f, 0.2f, 0.2f, 1.0f)));

	_sceneGraph->Add(Nose);

	SceneNodePointer LeftArm = SceneNodePointer(new CubeNode(L"LeftArm", Vector4(0.2f, 0.2f, 0.2f, 1.0f)));
	
	_sceneGraph->Add(LeftArm);

	SceneNodePointer RightArm = SceneNodePointer(new CubeNode(L"RightArm", Vector4(0.2f, 0.2f, 0.2f, 1.0f)));
	
	_sceneGraph->Add(RightArm);
	
	_rotationAngle = 0;
}

void DirectXApp::UpdateSceneGraph()
{
	SceneGraphPointer _sceneGraph = GetSceneGraph();
	_rotationAngle += 1.0f;
	
	_sceneGraph->SetWorldTransform(Matrix::CreateRotationY(_rotationAngle * XM_PI / 360));

	SceneNodePointer Teapot = _sceneGraph->Find(L"Teapot");
	Teapot->SetWorldTransform(Matrix::CreateScale(5, 5, 5) * Matrix::CreateTranslation(Vector3(20.0f, 15.0f, 0.0f)) * Matrix::CreateRotationY(_rotationAngle * XM_PI / 360));

	SceneNodePointer Nose = _sceneGraph->Find(L"Nose");
	Nose->SetWorldTransform(Matrix::CreateScale(1, 1, 1) * Matrix::CreateTranslation(Vector3(0.0f, 34.0f, 3.0f)) * Matrix::CreateRotationY(_rotationAngle * XM_PI / 360));

	SceneNodePointer Body = _sceneGraph->Find(L"Body");
	Body->SetWorldTransform(Matrix::CreateScale(5, 8, 2.5) * Matrix::CreateTranslation(Vector3(0.0f, 23.0f, 0.0f)) * Matrix::CreateRotationY(_rotationAngle * XM_PI / 360));
	
	SceneNodePointer LeftLeg = _sceneGraph->Find(L"LeftLeg");
	LeftLeg->SetWorldTransform(Matrix::CreateScale(1, 7.5, 1) * Matrix::CreateTranslation(Vector3(-4, 7.5, 0)) * Matrix::CreateRotationY(_rotationAngle * XM_PI / 360));
	
	SceneNodePointer RightLeg = _sceneGraph->Find(L"RightLeg");
	RightLeg->SetWorldTransform(Matrix::CreateScale(1, 7.5, 1) * Matrix::CreateTranslation(Vector3(4, 7.5, 0)) * Matrix::CreateRotationY(_rotationAngle * XM_PI / 360));
	
	SceneNodePointer Head = _sceneGraph->Find(L"Head");
	Head->SetWorldTransform(Matrix::CreateScale(3, 3, 3) * (Matrix::CreateTranslation(Vector3(0, 34, 0)) * Matrix::CreateRotationY(_rotationAngle * XM_PI / 360)));
	
	SceneNodePointer LeftArm = _sceneGraph->Find(L"LeftArm");
	LeftArm->SetWorldTransform(Matrix::CreateScale(1, 8.5, 1) * (Matrix::CreateTranslation(Vector3(-6, 22, 0)) * Matrix::CreateRotationY(_rotationAngle * XM_PI / 360)));
	
	SceneNodePointer RightArm = _sceneGraph->Find(L"RightArm");
	RightArm->SetWorldTransform(Matrix::CreateScale(1, 8.5, 1) * (Matrix::CreateTranslation(Vector3(6, 22, 0)) * Matrix::CreateRotationY(_rotationAngle * XM_PI / 360)));
	
	SceneNodePointer modelNode = _sceneGraph->Find(L"ModelNode");
	modelNode->SetWorldTransform(Matrix::CreateScale(5, 5, 5) * Matrix::CreateTranslation(Vector3(-25.0f, 15.0f, 0.0f)) * Matrix::CreateRotationY(_rotationAngle * XM_PI / 360));
}

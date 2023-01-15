#include "SceneGraph.h"


bool SceneGraph::Initialise(void)
{
	//For each child in the array, initalise. 
	for (int i = 0; i < _children.size(); i++)
	{
		if (!_children[i]->Initialise())
		{
			return false;
		}
	}
	return true;
}

void SceneGraph::Update(const Matrix& worldTransformation)
{
	//for each child in array, update with the worldTransformation
	for (int i = 0; i < _children.size(); i++)
	{
		_children[i]->Update(worldTransformation);
	}
}

void SceneGraph::Render(void)
{
	//For each child in array, render onto screen.
	for (int i = 0; i < _children.size(); i++)
	{
		_children[i]->Render();
	}

}

void SceneGraph::Shutdown(void)
{
	//for each child in arry, shutdown.
	for (int i = 0; i < _children.size(); i++)
	{
		_children[i]->Shutdown();
	}
}

void SceneGraph::Add(SceneNodePointer node)
{
	//push a new child to the array.
	_children.push_back(node);
}

void SceneGraph::Remove(SceneNodePointer node)
{
	//for each child in array, if it is the chosen node, remove from array.
	for (int i = 0; i < _children.size(); i++)
	{
		if (_children[i] == node)
		{
			_children[i]->Remove(node);
		}
	}
}

SceneNodePointer SceneGraph::Find(wstring name)
{
	//if parameter name is current point name, return this pointer.
	if (_name == name)
	{
		return SceneNodePointer(this);
	}

	// for each child in array, find the child and return its pointer.
	for (int i = 0; i < _children.size(); i++)
	{
		if (_children[i]->Find(name))
		{
			return SceneNodePointer(_children[i]);
		}
	}
	//else return a null pointer.
	return nullptr;
}

#include "Octree.h"
using namespace Simplex;
uint Octree::octantCount = 0;
uint Octree::idealEntityCount = 0;
uint Octree::maxLevel = 5;

/// <summary>
/// Instantiates the octree
/// </summary>
/// <param name="octantLevels">The current number of levels in the octree</param>
/// <param name="maxSub">The maximum number of levels in the octree</param>
Simplex::Octree::Octree(uint level, uint octantLevels, uint maxSub, MyEntityManager* eMngr, MeshManager* meshMngr)
{
	this->meshMngr = meshMngr;
	this->level = level;
	entityMngr = eMngr;
	maxLevel = maxSub;
	if (octantLevels > maxSub)
	{
		octantLevels = maxSub;
	}
	this->octantLevels = octantLevels;
	octantCount++;
	if (level == 1)
	{
		FindMinMaxCenter();
		for (uint i = 0; i < entityMngr->GetEntityCount(); i++)
		{
			entityList.push_back(i);
		}
		Subdivide();
	}
}


void Simplex::Octree::Subdivide()
{
	if (level < octantLevels && entityList.size() > idealEntityCount)
	{
		children = 8;
		for (uint i = 0; i < children; i++) //Create new octrees for each child
		{
			child[i] = new Octree(level + 1, octantLevels, maxLevel, entityMngr, meshMngr);
		}

		//Front square of subdivisions first
		child[0]->SetMin(v3Min); //Subdivision (0, 0, 0)
		child[0]->SetMax(v3Center);

		child[1]->SetMin(vector3(v3Center.x, v3Min.y, v3Min.z)); //Subdivision (1, 0, 0)
		child[1]->SetMax(vector3(v3Max.x, v3Center.y, v3Center.z));

		child[2]->SetMin(vector3(v3Min.x, v3Center.y, v3Min.z)); //Subdivision (0, 1, 0)
		child[2]->SetMax(vector3(v3Center.x, v3Max.y, v3Center.z));

		child[3]->SetMin(vector3(v3Center.x, v3Center.y, v3Min.z)); //Subdivision (1, 1, 0)
		child[3]->SetMax(vector3(v3Max.x, v3Max.y, v3Center.z));

		//Back square of subdivisions
		child[4]->SetMin(vector3(v3Min.x, v3Min.y, v3Center.z)); //Subdivision (0, 0, 1)
		child[4]->SetMax(vector3(v3Center.x, v3Center.y, v3Max.z));

		child[5]->SetMin(vector3(v3Center.x, v3Min.y, v3Center.z)); //Subdivision (1, 0, 1)
		child[5]->SetMax(vector3(v3Max.x, v3Center.y, v3Max.z));

		child[6]->SetMin(vector3(v3Min.x, v3Center.y, v3Center.z)); //Subdivision (0, 1, 1)
		child[6]->SetMax(vector3(v3Center.x, v3Max.y, v3Max.z));

		child[7]->SetMin(v3Center); //Subdivision (1, 1, 1)
		child[7]->SetMax(v3Max);

		for (uint n = 0; n < children; n++)
		{
			child[n]->SetCenter((child[n]->GetMax() + child[n]->GetMin()) / 2);
			child[n]->SetParent(this);
		}
		for (int q = entityList.size() - 1; q >= 0; q--) //Loops through the entities in the entity manager to see if they should be placed in one of the subdivisions
		{
			MyRigidBody* rb = entityMngr->GetRigidBody(q);
			for (uint n = 0; n < children; n++)
			{
				if (rb->GetMinGlobal().x > child[n]->GetMin().x && rb->GetMinGlobal().y > child[n]->GetMin().y && rb->GetMinGlobal().z > child[n]->GetMin().z) 
				{
					if (rb->GetMaxGlobal().x < child[n]->GetMax().x && rb->GetMaxGlobal().y < child[n]->GetMax().y && rb->GetMaxGlobal().z < child[n]->GetMax().z)
					{
						//If the Rigidbody is fully within the subdivision, places it within that subdivision
						child[n]->AddEntity(q);
						entityList.erase(entityList.begin() + q);
						break;
					}
				}
			}
		}
		if (level < octantLevels && entityList.size() > idealEntityCount)
		{
			for (uint n = 0; n < children; n++)
			{
				child[n]->Subdivide();
			}
		}
		
	}
}

void Simplex::Octree::SetLevel(uint level)
{
	this->level = level;
}

void Simplex::Octree::SetParent(Octree* parent)
{
	this->parent = parent;
}

void Simplex::Octree::Display()
{
	if (showBox)
	{
		glm::mat4 cubeTransform = glm::translate(v3Center);
		cubeTransform = glm::scale(cubeTransform, v3Max - v3Min);	
		meshMngr->AddWireCubeToRenderList(cubeTransform, C_YELLOW);
		if (children != 0)
		{
			for (uint i = 0; i < children; i++)
			{
				child[i]->Display();
			}
		}
	}
}

/// <summary>
/// Finds the min, max, and center of the octree based on the positions of the entities it contains
/// </summary>
void Simplex::Octree::FindMinMaxCenter()
{
	if (entityMngr == nullptr)
	{
		return;
	}
	for (int i = 0; i < entityMngr->GetEntityCount(); i++)
	{
		glm::mat4 modelMatrix = entityMngr->GetModelMatrix(i);
		float modelX = modelMatrix[3][0];
		float modelY = modelMatrix[3][1];
		float modelZ = modelMatrix[3][2];
		if (modelX < v3Min.x)
		{
			v3Min = vector3(modelX, v3Min.y, v3Min.z);
		}
		if (modelX > v3Max.x)
		{
			v3Max = vector3(modelX, v3Max.y, v3Max.z);
		}
		if (modelY < v3Min.y)
		{
			v3Min = vector3(v3Min.y, modelY, v3Min.z);
		}
		if (modelY > v3Max.y)
		{
			v3Max = vector3(v3Max.x, modelY, v3Max.z);
		}
		if (modelZ < v3Min.z)
		{
			v3Min = vector3(v3Min.y, v3Min.y, modelZ);
		}
		if (modelZ > v3Max.z)
		{
			v3Max = vector3(v3Max.x, v3Max.y, modelZ);
		}
	}
	v3Center = (v3Max + v3Min) / 2;
}

void Simplex::Octree::SetMin(vector3 min)
{
	v3Min = min;
}

vector3 Simplex::Octree::GetMin()
{
	return v3Min;
}

void Simplex::Octree::SetMax(vector3 max)
{
	v3Max = max;
}

vector3 Simplex::Octree::GetMax()
{
	return v3Max;
}

void Simplex::Octree::SetCenter(vector3 center)
{
	v3Center = center;
}

/// <summary>
/// Adds the index of an entity to the entity list
/// </summary>
/// <param name="entityIndex">The index of the entity to be addd</param>
void Simplex::Octree::AddEntity(uint entityIndex)
{
	entityList.push_back(entityIndex);
}

/// <summary>
/// Changes whether or not the boxes of every subdivision of the octree but the root are shown. Root does not need to change, as per the example.
/// </summary>
/// <param name="shown">Whether or not the boxes surrounding each subdivision are shown</param>
void Simplex::Octree::ShowBox(bool shown)
{
	if (level != 1)
	{
		showBox = shown;
	}
	if (children != 0)
	{
		for (int i = 0; i < children; i++)
		{
			child[i]->ShowBox(shown);
		}
	}
}

void Simplex::Octree::Update()
{
	if (level == 1) //Clear the collision list from the previous frame
	{
		for (uint i = 0; i < entityMngr->GetEntityCount(); i++)
		{
			entityMngr->GetEntity(i)->ClearCollisionList();
		}
	}
	//check collisions
	for (uint i = 0; i < entityList.size(); i++)
	{
		for (uint j = i + 1; j < entityList.size(); j++)
		{
			if (entityList[i] != entityList[j])
			{
				entityMngr->GetEntity(entityList[i])->IsColliding(entityMngr->GetEntity(entityList[j]));
			}
		}
		CheckChildCollisions(entityList[i]);
	}
	//Update children
	if (children != 0)
	{
		for (uint i = 0; i < children; i++)
		{
			child[i]->Update();
		}
	}
}

void Simplex::Octree::CheckChildCollisions(uint ID)
{
	//Checks an entity against any children that the octree it's in may have
	if (children != 0)
	{
		for (uint i = 0; i < children; i++)
		{
			child[i]->CheckCollisions(ID);
			child[i]->CheckChildCollisions(ID);
		}
	}
}

void Simplex::Octree::CheckCollisions(uint ID)
{
	for (uint i = 0; i < entityList.size(); i++)
	{
		if (ID != entityList[i]) //I'm not 100% sure why this was happening, but it was causing false positive collisions.
		{
			entityMngr->GetEntity(ID)->IsColliding(entityMngr->GetEntity(entityList[i]));
		}
	}
}

Simplex::Octree::~Octree()
{
	if (children != 0)
	{
		for (uint i = 0; i < children; i++)
		{
			delete child[i];
		}
	}
}

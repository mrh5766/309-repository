#pragma once

#include <vector>

#include "MyEntityManager.h"
namespace Simplex
{
	class Octree
	{
		static uint octantCount; //Number of octree instantiated
		static uint maxLevel; //Max level an octree can go to
		static uint idealEntityCount; //Ideal amount of entities for this object to contain
	private:
		std::vector<Octree*> nodes;

		uint ID = 0; //Current id for this octree
		uint level = 0; //Current level of the octree
		uint children = 0; //Number of children of the octree - 0 or 8
		uint octantLevels = 0; //Number of levels this octree has

		float size = 0.0f; //Size of the octree

		MeshManager* meshMngr = nullptr; //Mesh manager
		MyEntityManager* entityMngr = nullptr; //Entity manager

		vector3 v3Center = vector3(0.0f); //Center point of the octree
		vector3 v3Min = vector3(0.0f); //Minimum vector of the octree
		vector3 v3Max = vector3(0.0f); //Maximum vector of the octree
		
		Octree* parent = nullptr;
		Octree* child[8];

		bool showBox = true;

		std::vector<uint> entityList; //List of entities in this subdivision of the octree

		Octree* root = nullptr;

	public:
		Octree(uint level, uint octantLevels, uint maxSub, MyEntityManager* entityMngr, MeshManager* meshMngr); //construct octree
		//Model* model;
		void Subdivide(); //Subdivides the current node into 8 more octrees if above the ideal entity count and below the maxLevel
		void SetLevel(uint level); //Set the current subdivision level of the octree
		void SetParent(Octree* parent); //Set the parent of the octree
		void Display(); //display octree
		void FindMinMaxCenter(); //Finds the min, max, and center x,y,z based on the locations of the entities in the entity manager - should only be used for the root node
		void SetMin(vector3 min); //Set the min x,y,z of the node
		vector3 GetMin(); //Get the min x,y,z of the octree
		void SetMax(vector3 max); //Set the max x,y,z of the node
		vector3 GetMax(); //Get the max x,y,z of the octree
		void SetCenter(vector3 center); //Set the center x,y,z of the node
		void AddEntity(uint entityIndex);
		void ShowBox(bool shown); //Set whether or not to show the bounding box about the subdivisions of the octree
		void Update(); //Update collisions
		void CheckChildCollisions(uint ID); //Check an entity's collisions with the octree's children
		void CheckCollisions(uint ID); //Check an entity's collisions with all of the entities in an octree
		~Octree();
	};
}

#pragma once

#include <vector>


namespace Simplex
{
	class Octree
	{
	private:
		std::vector<Octree*> nodes;

	public:
		//Octree(uint octantLevels, uint maxSub); //construct octree
		//Model* model;
		//void Display(uint octreeID); //display octree
	};
}

#pragma once
#include "algorithm.h"

class Node
{
public:
	Node(int, IAlgorithm*, string);
	~Node();
private:
	int Id;
	vector<int>* VecNeiId;
	IAlgorithm* Algorithm;
	void ReadNeiId(string);
};



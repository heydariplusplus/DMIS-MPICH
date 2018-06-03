/*
Each node of the input network is an object of this class.
Each node has some attributes- a unique identifier (Id), a list of Ids of the neighbor nodes (VecNeiId), and an algorithm (Algorithm) 
that runs on the node.
*/

#pragma once
#include "algorithm.h"

class Node
{
public:
	Node(int, IAlgorithm*, string);	//the first parameter is the Id of the object, 
					//the second parameter is the algorithm that runs on each object
					//the third parameter is the path of the file that containes the neighbors' Ids
	~Node();
private:
	int Id;				//the unique identifier
	vector<int>* VecNeiId;		//the list of Ids of the neighbors
	IAlgorithm* Algorithm;		//an algorithm that runs on each object
	void ReadNeiId(string);		//a method for reading the neighbors' Ids from a file. The parameter is the path of that file.
};



#include "stdafx.h"
#include "node.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

Node::Node(int id, IAlgorithm* algorithm, string neiIdFilePath)
{
	this->VecNeiId = new vector<int>();
	this->Id = id;
	this->Algorithm = algorithm;

	this->ReadNeiId(neiIdFilePath);
	this->Algorithm->Run(this->Id, this->VecNeiId);
	this->Algorithm->ShowResult(this->Id);
}

Node::~Node()
{
	this->VecNeiId->clear();
	delete this->VecNeiId;		//free the memory
}

void Node::ReadNeiId(string neiIdFilePath)
{
	string line;
	ifstream neiIdFile;
	neiIdFile.open(neiIdFilePath, ios::in);
	
	if (neiIdFile.is_open())
	{
		while (getline(neiIdFile, line))
		{
			this->VecNeiId->push_back(stoi(line));	//convert string to int, and push it on the vector (list).
		}
	}
	else cout << "Node " << to_string(this->Id) << ":Unable to open input file" << endl;
	
	neiIdFile.close();
}


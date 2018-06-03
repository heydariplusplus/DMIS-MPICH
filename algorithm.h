/*
This class is an abstract class called IAlgorithm. 
Since some versions of c++ does not supprt interface (__interface), abstract class is used.
It is used for implementing a dependency injection. 
Objects of Node class can run some algorithms (note that in this program, each object of Node class runs only one algorithm called 
MisAlgorithm, but the Node class is written in a way that it could be further developed.), so dependency injection technique is usued.
*/

#pragma once
#include "stdafx.h"

class IAlgorithm
{
public:
	virtual void Run(int, vector<int>*) = 0;
	virtual void ShowResult(int) = 0;
};

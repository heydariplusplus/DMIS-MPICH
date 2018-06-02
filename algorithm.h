#pragma once
#include "stdafx.h"

class IAlgorithm
{
public:
	virtual void Run(int, vector<int>*) = 0;
	virtual void ShowResult(int) = 0;
};

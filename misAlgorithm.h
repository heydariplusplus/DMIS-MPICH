#pragma once
#include "algorithm.h"
#include "stdafx.h"

enum Status
{
	Mis,
	ComMis,
	Unknown
};

enum MessageTag
{
	RandNum,
	NodeStatus,
	IdComMis
};

class MisAlgorithm : public IAlgorithm
{
public:
	MisAlgorithm(string);
	virtual void Run(int, vector<int>*);
	virtual void ShowResult(int);
	Status GetNodeStatus();
private:
	Status NodeStatus;
	void Log(int, string,int=0);
	double Round1(int, vector<int>*);
	double Round2(int, vector<int>*);
	void Round3(int,vector<int>*,double&,double&);
	void Round4(int, vector<int>*);
	void Round5(int, vector<int>*);
	void Round6(int, vector<int>*);
	string LogDirectory;
};


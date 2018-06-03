#pragma once
#include "algorithm.h"
#include "stdafx.h"

//This enum is used to determine the status of each node of the network during the execution of the misAlgorithm
enum Status
{
	Mis,		//When a node joins to the MIS set, its status is Mis
	ComMis,		//When a node joins to the Complement of MIS set, its status is ComMis
	Unknown		//As long as the status of the node is specified, its status Unknown.
};

//This enum is used in the message passing duration to determine the types of messages.
enum MessageTag
{
	RandNum,
	NodeStatus,
	IdComMis
};

//This class implement the IAlgorithm abstract class (interface)
class MisAlgorithm : public IAlgorithm
{
public:
	MisAlgorithm(string);			//the parameter is the path of the file that the log (result) of the algorithm is written on it.
	virtual void Run(int, vector<int>*);
	virtual void ShowResult(int);
	Status GetNodeStatus();
private:
	Status NodeStatus;			//this field determines the status of the node
	void Log(int, string, int=0);		//the first parameter is the Id of the node,
						//the second parameter is the message that should be written to the log file,
						//the third parameter, which is an arbitrary parameter, is the iteration number of the algorithm
	double Round1(int, vector<int>*);	//the first round of each iteration of the algorithm
						//the first parameter is the Id of the node,
						//the second parameter is the list of neighbors' Ids
	double Round2(int, vector<int>*);	//the second round of each iteration of the algorithm
	void Round3(int, vector<int>*, double&, double&);	//the third round of each iteration of the algorithm
								//the third parameter is the output of the first round (Round1(,) method),
								//the fourth parameter is the output of the second round (Round2(,) method)
	void Round4(int, vector<int>*);		//the fourth round of each iteration of the algorithm
	void Round5(int, vector<int>*);		//the fifth round of each iteration of the algorithm
	void Round6(int, vector<int>*);		//the sixth round of each iteration of the algorithm
	string LogDirectory;			
};


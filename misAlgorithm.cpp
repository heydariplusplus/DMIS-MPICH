#include "stdafx.h"
#include "misAlgorithm.h"
#include <time.h>
#include <random>
#include <fstream>
#include <chrono>

//This function finds the maximum of an array
double Max(double* array, int lenght)
{
	double max = 0;
	for (int i = 0; i < lenght; ++i)if (max < array[i])max = array[i];
	return max;
}

//This function finds the index of an item in an array
int GetIndex(vector<int>* vec, int value)
{
	for (int i = 0; i < vec->size(); ++i)if (vec->at(i) == value)return i;
	return -1;
}

//This function generates a random double number
double RandomRange(int min, int max)
{
	mt19937 rng;
	rng.seed(std::random_device()());
	uniform_real_distribution<double> dist(min, max);
	return dist(rng);
}

MisAlgorithm::MisAlgorithm(string logDirectory)
{
	this->NodeStatus = Status::Unknown;
	this->LogDirectory = logDirectory;
}

void MisAlgorithm::Run(int id, vector<int>* vecNeiId)
{
	time_t  timev;
	time(&timev);

	Log(id, "---Start (" + to_string(timev) + ") ---");

	srand(time(NULL) + id);

	int iteration = 1;
	while (NodeStatus == Status::Unknown)	//the main loop of the algorithm. As long as the status of the node is Unknown (is not Mis or ComMis), this loop continues.
	{
		Log(id, "", iteration++);

		if (vecNeiId->size() == 0)	//if a node does not have any neighbor, it is added to the MIS set.
		{
			NodeStatus = Status::Mis;
			return;
		}

		double randNum = Round1(id, vecNeiId);
		double maxRecRandNum = Round2(id, vecNeiId);
		Round3(id, vecNeiId, randNum, maxRecRandNum);
		Round4(id, vecNeiId);
		Round5(id, vecNeiId);
		Round6(id, vecNeiId);
	}

	Log(id, "---Finish---");
}

//This method shows the result (status of the node after termination of the algorithm).
void MisAlgorithm::ShowResult(int id)
{
	string status;
	switch (this->NodeStatus)
	{
	case Status::Mis:
		status = "MIS";
		break;
	case Status::ComMis:
		status = "Complement of MIS";
		break;
	default:
		status = "Unknown";
	}

	string msg = "Node " + to_string(id) + " is " + status;
	cout << msg << endl;
	Log(id, msg);
}

Status MisAlgorithm::GetNodeStatus()
{
	return NodeStatus;
}

//This method writes the logs on a file
void MisAlgorithm::Log(int id, string message, int iteration)
{
	ofstream outFile;
	outFile.open(this->LogDirectory + "/" + to_string(id) + ".txt", ios_base::app);

	if (iteration == 0)outFile << message << "\n";
	else outFile << "*** Iteration" << iteration << " *** " << "\n";

	outFile.close();
}

//In this method, the node generates a random double number in [0, 1] and sends it to all of its neighbors  
double MisAlgorithm::Round1(int id, vector<int>* vecNeiId)
{
	Log(id, "...Round 1 Start...");

	double randNum = RandomRange(0, 1);		//generate a random number

	Log(id, " randNum: " + to_string(randNum));

	for (int i = 0; i < vecNeiId->size(); i++)
		MPI_Send(&randNum, 1, MPI_DOUBLE, vecNeiId->at(i), MessageTag::RandNum, MPI_COMM_WORLD);

	Log(id, "...Round 1 Finish...");

	return randNum;
}

//In this method, the node receives the generated random numbers of neighbors and find the maximum of them.
double MisAlgorithm::Round2(int id, vector<int>* vecNeiId)
{
	Log(id, "...Round 2 Start...");

	double* arrRecRandNum = new double[vecNeiId->size()];	//an array that will contain the random numbers of the neighbors
	for (int i = 0; i < vecNeiId->size(); i++)
	{
		double recRandNum;
		MPI_Recv(&recRandNum, 1, MPI_DOUBLE, vecNeiId->at(i), MessageTag::RandNum, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		arrRecRandNum[i] = recRandNum;
	}
	double maxRecRandNum = Max(arrRecRandNum, vecNeiId->size());	//find the maximum random number among reveived random numbers.
	delete[] arrRecRandNum;

	Log(id, " maxRecRandNum: " + to_string(maxRecRandNum));

	Log(id, "...Round 2 Finish...");

	return maxRecRandNum;
}

//In this method, if the random number of the node is greater than all of the neighbors' random numbers, the status of the node changes to Mis 
void MisAlgorithm::Round3(int id, vector<int>* vecNeiId, double& randNum, double& maxRecRandNum)
{
	Log(id, "...Round 3 Start...");

	if (maxRecRandNum < randNum)
		NodeStatus = Status::Mis;
	for (int i = 0; i < vecNeiId->size(); i++)
		MPI_Send(&NodeStatus, 1, MPI_INT, vecNeiId->at(i), MessageTag::NodeStatus, MPI_COMM_WORLD);

	Log(id, "...Round 3 Finish...");
}

void MisAlgorithm::Round4(int id, vector<int>* vecNeiId)
{
	Log(id, "...Round 4 Start...");

	Status* arrRecNodeStatus = new Status[vecNeiId->size()];
	for (int i = 0; i < vecNeiId->size(); i++)
	{
		Status recNodeStatus;
		MPI_Recv(&recNodeStatus, 1, MPI_INT, vecNeiId->at(i), MessageTag::NodeStatus, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		arrRecNodeStatus[i] = recNodeStatus;
	}
	if (NodeStatus != Status::Mis)
		for (int i = 0; i < vecNeiId->size(); i++)
			if (arrRecNodeStatus[i] == Status::Mis)
			{
				NodeStatus = Status::ComMis;
				break;
			}
	delete[] arrRecNodeStatus;

	Log(id, " NodeStatus: " + to_string(NodeStatus));

	Log(id, "...Round 4 Finish...");
}

void MisAlgorithm::Round5(int id, vector<int>* vecNeiId)
{
	Log(id, "...Round 5 Start...");

	if (NodeStatus == Status::ComMis)
		for (int i = 0; i < vecNeiId->size(); i++)
			MPI_Send(&id, 1, MPI_INT, vecNeiId->at(i), MessageTag::IdComMis, MPI_COMM_WORLD);
	else
	{
		int msg = -1;
		for (int i = 0; i < vecNeiId->size(); i++)
			MPI_Send(&msg, 1, MPI_INT, vecNeiId->at(i), MessageTag::IdComMis, MPI_COMM_WORLD);
	}

	Log(id, "...Round 5 Finish...");
}

void MisAlgorithm::Round6(int id, vector<int>* vecNeiId)
{
	Log(id, "...Round 6 Start...");

	if (NodeStatus == Status::Unknown)
	{
		vector<int> vecComMisId;
		for (int i = 0; i < vecNeiId->size(); i++)
		{
			int recId;
			MPI_Recv(&recId, 1, MPI_INT, vecNeiId->at(i), MessageTag::IdComMis, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (recId >= 0)vecComMisId.push_back(recId);
		}

		for (int i = 0; i < vecComMisId.size(); ++i)
		{
			int index = GetIndex(vecNeiId, vecComMisId.at(i));

			if (index >= 0)
			{
				vecNeiId->erase(vecNeiId->begin() + index, vecNeiId->begin() + index + 1);
			}
		}

		Log(id, "...Round 6 Finish...");
	}
}

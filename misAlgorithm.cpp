#include "stdafx.h"
#include "misAlgorithm.h"
#include <time.h>
#include <random>
#include <fstream>
#include <chrono>

double Max(double* array, int lenght)
{
	double max = 0;
	for (int i = 0; i < lenght; ++i)if (max < array[i])max = array[i];
	return max;
}

int GetIndex(vector<int>* vec, int value)
{
	for (int i = 0; i < vec->size(); ++i)if (vec->at(i) == value)return i;
	return -1;
}

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
	while (NodeStatus == Status::Unknown)
	{
		Log(id, "", iteration++);

		if (vecNeiId->size() == 0)
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
	cout << msg;
	Log(id, msg);
}

Status MisAlgorithm::GetNodeStatus()
{
	return NodeStatus;
}

void MisAlgorithm::Log(int id, string message, int iteration)
{
	ofstream outFile;
	outFile.open(this->LogDirectory + "/" + to_string(id) + ".txt", ios_base::app);

	if (iteration == 0)outFile << message << "\n";
	else outFile << "*** Iteration" << iteration << " *** " << "\n";

	outFile.close();
}

double MisAlgorithm::Round1(int id, vector<int>* vecNeiId)
{
	Log(id, "...Round 1 Start...");

	double randNum = RandomRange(0, 1);

	Log(id, " randNum: " + to_string(randNum));

	for (int i = 0; i < vecNeiId->size(); i++)
		MPI_Send(&randNum, 1, MPI_DOUBLE, vecNeiId->at(i), MessageTag::RandNum, MPI_COMM_WORLD);

	Log(id, "...Round 1 Finish...");

	return randNum;
}

double MisAlgorithm::Round2(int id, vector<int>* vecNeiId)
{
	Log(id, "...Round 2 Start...");

	double* arrRecRandNum = new double[vecNeiId->size()];
	for (int i = 0; i < vecNeiId->size(); i++)
	{
		double recRandNum;
		MPI_Recv(&recRandNum, 1, MPI_DOUBLE, vecNeiId->at(i), MessageTag::RandNum, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		arrRecRandNum[i] = recRandNum;
	}
	double maxRecRandNum = Max(arrRecRandNum, vecNeiId->size());
	delete[] arrRecRandNum;

	Log(id, " maxRecRandNum: " + to_string(maxRecRandNum));

	Log(id, "...Round 2 Finish...");

	return maxRecRandNum;
}

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

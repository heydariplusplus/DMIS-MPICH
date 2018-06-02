#include "stdafx.h"
#include "node.h"
#include "algorithm.h"
#include "misAlgorithm.h"

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(argc==3)
	{
		string inputDirectory = argv[1];
		string inputPath = inputDirectory + "/" + to_string(rank) + ".txt";
		string logDirectory = argv[2];

		Node* node = new Node(rank, new MisAlgorithm(logDirectory), inputPath);
	}
	else
	{
		cout << "Command line arguments are incorrect";
	}

	MPI_Finalize();

	return 0;
}

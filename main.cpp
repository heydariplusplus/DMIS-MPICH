#include "stdafx.h"
#include "node.h"
#include "algorithm.h"
#include "misAlgorithm.h"

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);	

	int rank;	//the unique identifier is assigned to the process.
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(argc == 3)	//check if the entered command is valid or not
	{
		string inputDirectory = argv[1];	
		string inputPath = inputDirectory + "/" + to_string(rank) + ".txt";
		string logDirectory = argv[2];

		Node* node = new Node(rank, new MisAlgorithm(logDirectory), inputPath);	//instantiate an object from class Node	
	}
	else
	{
		cout << "Command line arguments are incorrect";
	}

	MPI_Finalize();	 

	return 0;
}

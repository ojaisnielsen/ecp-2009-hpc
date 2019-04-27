#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
	int rank, size, reicv, mess, sender, dest, i, nLoops;
	MPI_Status status;



	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	sender = rank - 1;				// Dans le cas général, le rang du processus dont on recoit le message est le précédent
	dest = rank + 1;				// Dans le cas général, le rang du processus auquel on anvoie le message est le suivant
	if (rank == 0)					// 1er cas particulier : le processus 0 recoit du dernier processus.
	{
		sender = size - 1;
	}
	if (rank = size - 1)			// 2eme cas particulier : le dernier processus ancoie au premier
	{
		dest = 0;
	}

				

	if (rank == 0)
	{
		scanf("%d", &nLoops);						// Le premier processus demande à l'utilisateur le nombre de boucles (nLoops)
		for (i = 1; i < size; i++)
		{
			MPI_Send(&nLoops, 1, MPI_INTEGER, i, 0, MPI_COMM_WORLD);	// Puis, il l'envoie à tous les autres...
		}
	}
	else
	{
		MPI_Recv(&nLoops, 1, MPI_INTEGER,0, 0, MPI_COMM_WORLD, &status); // ...qui reçoivent ce nombre
	}

	reicv = 0;	
	for (i = 0; i < nLoops, i++)			// A chaque itération
	{
		if (i == 0 && rank == 0) 
		{
			
			MPI_Send(&rank, 1, MPI_INTEGER, dest, 0, MPI_COMM_WORLD); // Au premier tour, le premier processus envoie son rang au suivant
		}
		else 
		{
			MPI_Recv(&reicv, 1, MPI_INTEGER, sender, 0, MPI_COMM_WORLD, &status); // Sinon, on commence par recevoir le message du précédent
		}

		mess = reicv + rank;			// On y ajoute son rang
		MPI_Send(&mess, 1, MPI_INTEGER, dest, 0, MPI_COMM_WORLD);  // et l'on renvoie au suivant
		printf("I am %d, I get %d from %d\n", rank, reicv, sender); // et on affiche le message recu
	}

	MPI_Finalize();
	return 0;
}
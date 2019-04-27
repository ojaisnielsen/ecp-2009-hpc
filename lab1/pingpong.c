#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
	int rank, size, reicv;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)                                                            // Le processus n�0 envoie en premier
	{
		MPI_Send(&rank, 1, MPI_INTEGER, 1, 0, MPI_COMM_WORLD);                // On envoie la valeur du rang, la quantit� est donc 1 de type entier, le destinataire est le processus 1, le flag, non utilis� est mis � 0, et la communaut� est l'ensemble total.
		MPI_Recv(&reicv, 1, MPI_INTEGER, 1, 0, MPI_COMM_WORLD, &status);	  // De m�me on r�cup�re le message recu dans reicv et le sattus dans status.
	}
	else
	{
		MPI_Recv(&reicv, 1, MPI_INTEGER, 0, 0, MPI_COMM_WORLD, &status);	// Le processus n�1 attend de recevoir le message de l'autre processus avant d'envoyer le sien.
		MPI_Send(&rank, 1, MPI_INTEGER, 0, 0, MPI_COMM_WORLD);
	}
	printf("I am %d, I get %d\n", rank, reicv); // Enfin, on affiche le message re�u
	MPI_Finalize();
	return 0;
}
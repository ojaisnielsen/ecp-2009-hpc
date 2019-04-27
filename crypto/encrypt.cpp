#include <CImg.h>
#include <mpi.h>

using namespace cimg_library;

int main(int argc, char *argv[])
{

	int rank;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	char *fileName = argv[1];
	//CImg<unsigned char> im(fileName);
	CImg<unsigned char> im("image_claire.jpg");
	int l = im.dimy();
	int c = im.dimx();
	im.resize(c, l, 1, 1);
	
	int n = 3;
	CImg<int> theta(1, 3, 1, 1, "1, 7, 4", 0);
	CImg<int> A = theta;
	A.append(CImg<int>(1, 3, 1, 1, "1, 0, 0", 0), 'x');
	A.append(CImg<int>(1, 3, 1, 1, "0, 1, 0", 0), 'x');

	CImg<int> B(1, n, 1, 1, 0);
	B(0, 0) = 1;

	CImg<int> C(3, 1, 1, 1, "1, 0, 0", 0);

	int mo = 256;
	CImg<int> Mo(1, 3, 1, 1, mo);

	//CImg<double> m(c, l, 1, 1, 0);
	CImg<double> y(c, l, 1, 1, 0);

	CImg<unsigned char> X_k(1, 3, 1, 1, "3, 11, 18", 0);

	if (rank == 0)
	{

		for (int k1 = 0; k1 < l; k1++)
		{
			for (int k2 = 0; k2 < c; k2++)
			{
				int mess[3];
				mess[0] = X_k(0, 0);
				mess[1] = X_k(0, 1);
				mess[2] = X_k(0, 2);
				MPI_Send(mess, 3, MPI_INTEGER, 1, 0, MPI_COMM_WORLD); 

				double u = (double) (im(k2, k1)^X_k(0, 0));
				CImg<unsigned char> X_k1 = ((A * X_k) + (B * u)) % Mo;
				//y(k2, k1) = (C*X_k)(0, 0) % mo;
				X_k = X_k1;
			}
		}

	}
	else
	{
		for (int k1 = 0; k1 < l; k1++)
		{
			for (int k2 = 0; k2 < c; k2++)
			{
				int mess[3];
				MPI_Recv(mess, 3, MPI_INTEGER, 0, 0, MPI_COMM_WORLD, &status); 
				X_k(0, 0) = mess[0];
				X_k(0, 1) = mess[1];
				X_k(0, 2) = mess[2];

				y(k2, k1) = (C*X_k)(0, 0) % mo;
			}
		}
		y.save("crypted.jpg");
	}
	MPI_Finalize();
	return 0;
}
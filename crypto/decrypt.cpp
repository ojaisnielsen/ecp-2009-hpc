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
	//CImg<double> y(fileName);
	CImg<unsigned char> y("crypted.jpg");
	int l = y.dimy();
	int c = y.dimx();
	y.resize(c, l, 1, 1);
	
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

	double CBi = 1. / ((C * B)(0, 0));
	CImg<double> P = A - (B * CBi * C * A);

	CImg<double> X_est_k(1, 3, 1, 1, "1, 4, 220", 0);

	CImg<double> m = y;

	if (rank == 0)
	{

		for (int k1 = 0; k1 < l; k1++)
		{
			for (int k2 = 0; k2 < c - 1; k2++)
			{
				int mess[3];
				mess[0] = X_est_k(0, 0);
				mess[1] = X_est_k(0, 1);
				mess[2] = X_est_k(0, 2);
				MPI_Send(mess, 3, MPI_INTEGER, 1, 0, MPI_COMM_WORLD); 

				int K1 = k1 + 1;
				int K2 = k2 + 1;
				int y_k2 = ((K2 + 1) % (c + 1)) + ((K2 + 1) / (c + 1)) - 1;
				int y_k1 = (K1 + ((K2 + 1) / (c + 1))) - 1;
				CImg<double> X_est_k1 = ((P * X_est_k) + (B * CBi * y(y_k2, y_k1))) % Mo;
				int u_est = (int)(CBi * (y(y_k2, y_k1) - ((C * A * X_est_k)(0, 0)))) % mo;
				m(k2, k1) = u_est^((int) (X_est_k(0, 0)));
				X_est_k = X_est_k1;
			}
		}
	}
	else
	{
		for (int k1 = 0; k1 < l; k1++)
		{
			for (int k2 = 0; k2 < c - 1; k2++)
			{
				int mess[3];
				MPI_Recv(mess, 3, MPI_INTEGER, 0, 0, MPI_COMM_WORLD, &status); 
				X_est_k(0, 0) = mess[0];
				X_est_k(0, 1) = mess[1];
				X_est_k(0, 2) = mess[2];

				int K1 = k1 + 1;
				int K2 = k2 + 1;
				int y_k2 = ((K2 + 1) % (c + 1)) + ((K2 + 1) / (c + 1)) - 1;
				int y_k1 = (K1 + ((K2 + 1) / (c + 1))) - 1;
				int u_est = (int)(CBi * (y(y_k2, y_k1) - ((C * A * X_est_k)(0, 0)))) % mo;
				m(k2, k1) = u_est^((int) (X_est_k(0, 0)));
			}
		}

		m.save("decrypted.jpg");
	}
	MPI_Finalize();

	return 0;
}
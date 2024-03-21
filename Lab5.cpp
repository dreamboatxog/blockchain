#include <iostream>
#include <mpi.h>
#include <ctime>
#include <cstdlib>

using namespace std;
const int N = 3; // Размерность матрицы
double A[N][N + 1], B[N][N + 1];

void gauss(double A[N][N + 1]) {
	// Прямой ход
	for (int k = 0; k < N - 1; k++) {

		for (int i = k + 1; i < N; i++) {
			double koef = A[i][k] / A[k][k];
			for (int j = k; j < N + 1; j++) {
				A[i][j] -= koef * A[k][j];
			}
		}
	}
	// Обратный ход
	for (int k = N - 1; k >= 0; k--) {
		A[k][N] /= A[k][k];
		A[k][k] = 1.0;
		for (int i = 0; i < k; i++) {
			A[i][N] -= A[i][k] * A[k][N];
			A[i][k] = 0.0;
		}
	}
	for (int i = 0; i < N; ++i) {
		cout << "x[" << i << "] = " << A[i][N] << endl;
	}
}

int main(int argc, char* argv[]) {	
	int rank, size;
	int localRows;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	localRows = N / size;//Кол-во строк на процесс
	if (rank == 0)
	{
		double values[N][N + 1] = { {1, 2, -1, 2}, {2, -3, 2, 2}, {3, 1, 1, 8} };
		//double values[N][N + 1] = { {1, 1, 2, 3, 1}, { 1, 2, 3, -1,-4 }, { 3, -1, -1, -2,-4 }, { 2, 3, -1, -1,-6 } };
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N + 1; ++j) {
				A[i][j] = values[i][j];//инициализируем матрицу на 0 процессе 
				B[i][j] = values[i][j];//инициализируем матрицу на 0 процессе 
			}
		}

		/*srand(time(0));
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N + 1; ++j) {
				A[i][j] = rand() % 10 + 1;
				B[i][j] = A[i][j];
			}
		}*/

	}
	MPI_Bcast(&A[0][0], N * (N + 1), MPI_DOUBLE, 0, MPI_COMM_WORLD);


	// Прямой ход
	int start = rank * localRows;
	int end = start + localRows;
	double temp[N][N + 1];
	for (int k = 0; k < N - 1; k++) {
		if (end - 1 > k) {
			cout << "k " << k << " rank" << rank << endl;
			for (int i = start; i < end; i++) {
				if (i > k) {
					double koef = A[i][k] / A[k][k];
					for (int j = k; j < N + 1; j++) {
						A[i][j] -= koef * A[k][j];
					}
				}

			}
		}

		MPI_Gather(&A[start][0], localRows * (N + 1), MPI_DOUBLE, &temp[start][0], localRows * (N + 1), MPI_DOUBLE, 0, MPI_COMM_WORLD);//собираем изменения во временный буфер 
		if (rank == 0)
		{
			for (int i = 0; i < N; i++)
			{
				for (int j = 0; j < N + 1; j++)
				{
					A[i][j] = temp[i][j];//актуализируем данные в матрцице A
				}
			}
		}
		MPI_Bcast(&A[0][0], N * (N + 1), MPI_DOUBLE, 0, MPI_COMM_WORLD);//рассылаем с 0 процесса данные всем остальным процессам с целью актуализации
	}
	if (rank == 0)
	{
		cout << endl << " Results after a forward elimination " << endl;
		for (int f = 0; f < N; ++f) {
			for (int r = 0; r < N + 1; ++r) {
				cout << A[f][r] << " ";
			}
			cout << endl;
		}
	}
	// Обратный ход
	for (int k = N - 1; k >= 0; k--) {
		if (rank == 0)
		{
			A[k][N] /= A[k][k];
			A[k][k] = 1.0;
		}
		MPI_Bcast(&A[k][0], (N + 1), MPI_DOUBLE, 0, MPI_COMM_WORLD);//рассылаем с 0 процесса данные всем остальным процессам с целью актуализации
		for (int i = start; i < end; i++) {
			if (i < k)
			{
				cout << "kBack " << k << " rankBack " << rank << endl;
				A[i][N] -= A[i][k] * A[k][N];
				A[i][k] = 0.0;
			}
		}
		MPI_Gather(&A[start][0], localRows * (N + 1), MPI_DOUBLE, &temp[start][0], localRows * (N + 1), MPI_DOUBLE, 0, MPI_COMM_WORLD);//собираем изменения во временный буфер 
		if (rank == 0)
		{
			for (int i = 0; i < N; i++)
			{
				for (int j = 0; j < N + 1; j++)
				{
					A[i][j] = temp[i][j];//актуализируем данные в матрцице A
				}
			}
		}
		MPI_Bcast(&A[0][0], N * (N + 1), MPI_DOUBLE, 0, MPI_COMM_WORLD);//рассылаем с 0 процесса данные всем остальным процессам с целью актуализации

	}
	if (rank == 0) {	
		cout << endl << " Final matrix " << endl;
		for (int f = 0; f < N; ++f) {
			for (int r = 0; r < N + 1; ++r) {
				cout << A[f][r] << " ";
			}
			cout << endl;
		}
	}

	// Вывод результатов на процессе с рангом 0
	if (rank == 0) {
		cout << endl;
		for (int i = 0; i < N; ++i) {
			cout << "x[" << i << "] = " << A[i][N] << endl;
		}
		cout << endl;
	}

	// Решение слау для проверки результата работы параллельной части
	if (rank == 0) {
		gauss(B);
	}
	MPI_Finalize();
	return 0;
}
#include "mpi.h"
#include <ctime>
#include <iostream>
using namespace std;

const int N = 10;
int B[N][N], A[N][N];
int C[N][N];
void setMatrix(int matrix[N][N]) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = rand() % 10 - 1; 
        }
    }
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "RU");
    int i, j, k, rank, size, sum = 0;
    int localA[N], localC[N];
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        setMatrix(A);

        cout << "Matrix A:" << endl;
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                cout << A[i][j] << " ";
            }
            cout << endl;
        }
        setMatrix(B);
        cout << "Matrix B:" << endl;
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                cout << B[i][j] << " ";
            }
            cout << endl;
        }

    }  
    MPI_Scatter(A, N * N / size, MPI_INT, localA, N * N / size, MPI_INT, 0, MPI_COMM_WORLD);//Распределяем каждому процессу по одной строке матрицы
    MPI_Bcast(B, N * N, MPI_INT, 0, MPI_COMM_WORLD);//Распределяем каждому процессу по одной строке матрицы
    
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            sum = sum + localA[j] * B[j][i];  
            //cout << " localA[" << j << "] " << localA[j] << " * B[" << j << "][" << i << "]" <<B[j][i]<<endl;
        }
        //cout << "rank: " << rank << " i: " << i << " sum: " << sum <<endl ;
        localC[i] = sum;
        sum = 0;
    }
    
    cout<<"proc "<< rank <<" data: ";//Номер процесса и данные с которыми он работает (localA)
    for (i = 0; i < N ; i++) {
        cout<<localA[i]<<" ";
    }
    cout<<"\n";
    
    MPI_Barrier(MPI_COMM_WORLD);//Ождиаем пока все процессы выполнят код до текущей строки для синхронизации
    
    MPI_Gather(localC, N * N / size, MPI_INT, C, N * N / size, MPI_INT, 0, MPI_COMM_WORLD);//Сбор данных из каждого процесса в финальную матрицу C

    if (rank == 0)//Вывод финальной матрицы после синхронизации и сбора данных 
    {
        cout << "Matrix C:" << endl;
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                cout << C[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl << endl;
    }
    MPI_Finalize();

}


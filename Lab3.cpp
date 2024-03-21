#include <iostream>
#include <omp.h>
#include <ctime>
#include <cstdlib>

using namespace std;
const int N = 1000; // Размерность матрицы

void gaussianOpenMP(double A[N][N + 1]) {
    int i, j, k;
    double koef;

    // Прямой ход
    for (k = 0; k < N - 1; k++) {
#pragma omp parallel for private(j, koef) shared(A, k) schedule(dynamic)
        for (i = k + 1; i < N; i++) {
            koef = A[i][k] / A[k][k];
            for (j = k; j < N + 1; j++) {
                A[i][j] -= koef * A[k][j];
            }
        }
    }

    // Обратный ход
    for (k = N - 1; k >= 0; k--) {
        A[k][N] /= A[k][k];
        A[k][k] = 1.0;
#pragma omp parallel for  schedule(dynamic)
        for (i = 0; i < k; i++) {
            A[i][N] -= A[i][k] * A[k][N];
            A[i][k] = 0.0;
        }
    }
    for (int i = 0; i < 10; ++i) {
        cout << "x[" << i << "] = " << A[i][N] << endl;
    }//Вывод первых 10 элементов для сравенения результатов
}

void gaussian(double A[N][N + 1]) {
    int i, j, k;
    double koef;

    // Прямой ход
    for (k = 0; k < N - 1; k++) {
        for (i = k + 1; i < N; i++) {
            koef = A[i][k] / A[k][k];
            for (j = k; j < N + 1; j++) {
                A[i][j] -= koef * A[k][j];
            }
        }
    }

    // Обратный ход
    for (k = N - 1; k >= 0; k--) {
        A[k][N] /= A[k][k];
        A[k][k] = 1.0;
        for (i = 0; i < k; i++) {
            A[i][N] -= A[i][k] * A[k][N];
            A[i][k] = 0.0;
        }
    }
    for (int i = 0; i < 10; ++i) {
        cout << "x[" << i << "] = " << A[i][N] << endl;
    }//Вывод первых 10 элементов для сравенения результатов
}

int main() {
    setlocale(LC_ALL, "Russian");
    double A[N][N + 1]; 
    double B[N][N + 1];

    srand(time(0));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N + 1; ++j) {
            A[i][j] = rand() % 10 + 1; 
            B[i][j] = A[i][j];
        }
    }

    double start_sequential = omp_get_wtime();
    gaussian(A);
    double end_sequential = omp_get_wtime();
    cout << "Время при последовательном выполнении: " << end_sequential - start_sequential << " с." << endl;

    
    double start_parallel = omp_get_wtime();
    gaussianOpenMP(B);
    double end_parallel = omp_get_wtime();
    cout << "Время при распараллеленном выполнении: " << end_parallel - start_parallel << " с." << endl;
    
    return 0;
}

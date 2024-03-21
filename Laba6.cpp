#include "iostream"
#include <tbb/tbb.h>

using namespace std;
const int N = 1500;

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
	for (int i = 0; i < 10; ++i) {
		cout << "x[" << i << "] = " << A[i][N] << endl;
	}
}
void gaussTbb(double A[N][N + 1]) {
	// Прямой ход
	for (int k = 0; k < N - 1; k++) {
		tbb::parallel_for(k+ 1, N, [&](int i) {
			double koef = A[i][k] / A[k][k];
			for (int j = k; j < N + 1; j++) {
				A[i][j] -= koef * A[k][j];
			}
			//cout << "Прямой ход Поток " << tbb::this_task_arena::current_thread_index() << " обработал строку " << i << " k= "<<k<<endl;
			});

	}
	//for (int i = 0; i < N; ++i) {
	//	        for (int j = 0; j < N+1; ++j) {
	//	            std::cout << A[i][j] << "\t";
	//	        }
	//	        std::cout << std::endl;
	//	    }
	// Обратный ход
	for (int k = N - 1; k >= 0; k--) {
		A[k][N] /= A[k][k];
		A[k][k] = 1.0;
		tbb::parallel_for(0, k, [&](int i) {
			A[i][N] -= A[i][k] * A[k][N];
			A[i][k] = 0.0;
			//cout << "Обратный ход Поток " << tbb::this_task_arena::current_thread_index() << " обработал строку " << i << " k= " << k<< endl;
			});
	}
	/*for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N + 1; ++j) {
			std::cout << A[i][j] << "\t";
		}
		std::cout << std::endl;
	}*/


	for (int i = 0; i < 10; i++) {
		std::cout << "x[" << i << "] = " << A[i][N] << std::endl;
	}
}

int main() {
	setlocale(LC_ALL, "Russian");
	//double a[N][N + 1] = { {1, 1, 2, 3, 1}, { 1, 2, 3, -1,-4 }, { 3, -1, -1, -2,-4 }, { 2, 3, -1, -1,-6 } };
	double A[N][N + 1] = { {1, 2, -1, 2}, {2, -3, 2, 2}, {3, 1, 1, 8} };
	double B[N][N + 1] = { {1, 2, -1, 2}, {2, -3, 2, 2}, {3, 1, 1, 8} };
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N + 1; ++j) {
			A[i][j] = rand() % 10 + 1;
			B[i][j] = A[i][j];
		}
	}
	tbb::tick_count startTbb = tbb::tick_count::now(); // Засекаем начало времени выполнения параллельной функции
	gaussTbb(A);
	tbb::tick_count endTbb = tbb::tick_count::now(); // Засекаем конец времени выполнения параллельной функции
	cout << "OneTbb Затрачено времени: " << (endTbb - startTbb ).seconds() << " сек." << endl; 

	tbb::tick_count start = tbb::tick_count::now(); // Засекаем начало времени
	gauss(B);
	tbb::tick_count end= tbb::tick_count::now(); // Засекаем конец времени
	cout << "Затрачено времени: " << ( end- start).seconds() << " сек." << endl; // Выводим время выполнения
	return 0;
}

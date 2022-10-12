#include<iostream>
#include<algorithm>
#include<random>
#include<time.h>
#include<mpi.h>
#define N 16		//num_elements
#define M 1000 
using namespace std;

int find_partner(int my_rank, int phase) {
	if (phase % 2 == 0) {
		if (my_rank % 2 == 0) {
			return my_rank - 1;
		}
		else {
			return my_rank + 1;
		}
	}
	else {
		if (my_rank % 2 == 0) {
			return my_rank + 1;
		}
		else {
			return my_rank - 1;
		}
	}
}
 
void generate_rand_array(int A[], int local_n, int my_rank) {
	int* a = NULL;
	if (my_rank == 0) {
		a = new int[N];
		srand((int)time(0));
		for (int i = 0; i < N; i++) {
			a[i] = rand() % M;
		}
        cout << "The unsorted array is:";
        cout << endl;
        for (int j = 0; j < N; j++) {
            cout << a[j] << " ";
        }
        cout << endl;
		MPI_Scatter(a, local_n, MPI_INT, A, local_n, MPI_INT, 0, MPI_COMM_WORLD);
		delete[] a;
	}
	else {
		MPI_Scatter(a, local_n, MPI_INT, A, local_n, MPI_INT, 0, MPI_COMM_WORLD);
	}
}


void merge_low(int my_keys[], int recv_keys[], int local_n) {
	int* temp_keys = new int[local_n];		
	int m_i = 0, r_i = 0, t_i = 0;	
 
 	while (t_i < local_n) {
		if (my_keys[m_i] <= recv_keys[r_i]) {
			temp_keys[t_i] = my_keys[m_i];
            t_i++;
            m_i++;
		}
		else {
			temp_keys[t_i] = recv_keys[r_i];
            t_i++;
            r_i++;
		}
	}
	for (m_i = 0; m_i < local_n; m_i++) {
		my_keys[m_i] = temp_keys[m_i];
	}
	delete[] temp_keys;
}
 
void merge_high(int my_keys[], int recv_keys[], int local_n) {
	int* temp_keys = new int[local_n];
	int m_i = local_n - 1, r_i = local_n - 1, t_i = local_n - 1;
	while (t_i >= 0) {
		if (my_keys[m_i] > recv_keys[r_i]) {
			temp_keys[t_i] = my_keys[m_i];
            t_i--;
            m_i--;
		}
		else {
			temp_keys[t_i] = recv_keys[r_i];
            t_i--;
            r_i--;
		}
	}
	for (m_i = 0; m_i < local_n; m_i++) {
		my_keys[m_i] = temp_keys[m_i];
	}
	delete[] temp_keys;
}
 
void print_array(int A[], int local_n, int my_rank) {
	int* a = NULL;
	if (my_rank == 0) {
		a = new int[N];
		MPI_Gather(A, local_n, MPI_INT, a, local_n, MPI_INT, 0, MPI_COMM_WORLD);
		cout << "The sorted array is:";
        cout << endl;
        for (int i = 0; i < N; i++) {
			cout << a[i] << " ";
		}
		cout << endl;
		delete[] a;
	}
	else {
		MPI_Gather(A, local_n, MPI_INT, a, local_n, MPI_INT, 0, MPI_COMM_WORLD);
	}
}
 
int main(int argc, char *argv[]) {
	int local_n;	
	int* A, * B;	
	int comm_sz;
    int my_rank;
    double start, end;
	MPI_Init(&argc, &argv);
 
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
 
	local_n = N / comm_sz;
	A = new int[local_n];
	B = new int[local_n];
 
	generate_rand_array(A, local_n, my_rank);
    start = MPI_Wtime()
	sort(A, A + local_n);
 
	for (int phase = 0; phase < comm_sz; phase++) {
		int partner = find_partner(my_rank, phase);
		if (partner != -1 && partner != comm_sz) {
			MPI_Sendrecv(A, local_n, MPI_INT, partner, 0, B, local_n, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
			if (my_rank < partner) {
				merge_low(A, B, local_n);
			}
			else {
				merge_high(A, B, local_n);
			}
		}
	}
 
	print_array(A, local_n, my_rank);
    end = MPI_Wtime()
    cout << "Student ID: " << "119010239" << endl; // replace it with your student id
    cout << "Name: " << "潘涛" << endl; // replace it with your name
    cout << "Assignment 1" << endl;
    cout << "Run Time: " << end-start << " seconds" << endl;
    cout << "Input Size: " << N << endl;
    cout << "Process Number: " << comm_sz << endl; 
    delete[] A;
    delete[] B;
	MPI_Finalize();
	return 0;
}

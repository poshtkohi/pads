/**
    #define meta ...
    printf("%s\n", meta);
**/

/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2021.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include <iostream>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <sys/time.h>
#include <mpi.h>
#include <Sparse/Sequential/SparseMatrix.h>

using namespace std;
using namespace Supercomputing::Sparse::Sequential;
//---------------------------------------
struct info_load_t
{
    int m;
    int n;
    int decrement;
    int nzmax;
    int nnz;
    std::vector<int> p;
    std::vector<int> i;
    std::vector<double> x;
    std::vector<double> v; // multiplied-by vector
    std::vector<double> r; // result vector
};
//---------------------------------------
int setup_mpi_environment();
int distribute_data(int, info_load_t *);
int collect_distributed_data(int, info_load_t *);
void perform_block_multiplication_vector(int, info_load_t *);
void cleanup_mpi_environment(info_load_t *);
info_load_t *load_matrix();
template <typename T>
void load_vector(std::vector<T> &, const std::string &);
void sequential_multiplication();
void print_execution_time(struct timeval &, struct timeval &);
void print_matrix(info_load_t *, const std::string &, int);
void print_vector(const std::vector<double> &, const std::string &, int);
//---------------------------------------
int main(int argc, char **argv)
{
    info_load_t *A_block = new info_load_t;
    struct timeval start, stop;

    int my_rank = setup_mpi_environment();

    if(my_rank == 0 || my_rank == -2)
        gettimeofday(&start, NULL);

    if(my_rank == -1)
        goto Cleanup;
    else if (my_rank == -2)
    {
        sequential_multiplication();
        goto Cleanup;
    }

    //printf("my_rank=%d\n", my_rank);
    if(distribute_data(my_rank, A_block) == -1)
        goto Cleanup;

    if(my_rank > 0)
    {
        perform_block_multiplication_vector(my_rank, A_block);
        //goto Cleanup;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    collect_distributed_data(my_rank, A_block);

    if(my_rank == 0)
    {
        ///print_vector(A_block->r, "r", my_rank);
    }

Cleanup:

    if(my_rank == 0 || my_rank == -2)
    {
        gettimeofday(&stop, NULL);
        print_execution_time(start, stop);
    }

    cleanup_mpi_environment(A_block);

    return 0;
}
//---------------------------------------
int setup_mpi_environment()
{
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if(world_size == 1)
        return -2;

    if(world_size < 3)
    {
        const std::string err_str = "The number of MPI processes must be at least 3";
        printf("%s\n", err_str.c_str());
        throw std::range_error(err_str);
        cleanup_mpi_environment(NULL);
        return -1;
    }

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    return world_rank;
}
//---------------------------------------
void cleanup_mpi_environment(info_load_t *A_block)
{
    if(A_block != NULL)
        delete A_block;

    MPI_Finalize();
}
//---------------------------------------
int distribute_data(int my_rank, info_load_t *A_block)
{
    if(A_block == NULL)
    {
        const std::string err_str = "A_block could not be null";
        throw std::runtime_error(err_str);
        return -1;
    }

    if(my_rank == 0)
    {
        // Get the number of processes
        int world_size;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        //printf("world_size=%d\n", world_size);

        info_load_t *mat = load_matrix();
        SparseMatrix *A = new SparseMatrix(mat->m, mat->n, mat->p, mat->i, mat->x);
        std::vector<double> &v = mat->v;

        /*SparseMatrix *A = new SparseMatrix(5, 5);
        A->Set(0, 0, 1);
        A->Set(1, 2, -1);
        A->Set(2, 0, 2);
        A->Set(2, 2, 1);
        A->Set(3, 1, -1);
        A->Set(4, 4, 3); //

        std::vector<double> v = std::vector<double>(5);
        v[0] = 1;
        v[1] = 2;
        v[2] = 3;
        v[3] = 4;
        v[4] = 5;*/

        if(A->GetN() != v.size())
        {
            delete A;
            ///delete mat;
            const std::string err_str = "Size of v must be equal to the number of columns of A matrix!";
            //printf("%s\n", err_str.c_str());
            throw std::range_error(err_str);
            //cleanup_mpi_environment();
            return -1;
        }

        if(A->GetN() < world_size - 1)
        {
            delete A;
            ///delete mat;
            const std::string err_str = "Matrix dimension is less than the number of MPI processes minus one";
            //printf("%s\n", err_str.c_str());
            throw std::range_error(err_str);
            //cleanup_mpi_environment();
            return -1;
        }

        A_block->m = A->GetM(); A_block->n = A->GetN();

        // Send column blocks of A
        int a = A->GetN() / (world_size - 1);
        int b = A->GetN() % (world_size - 1);

        ///printf("a=%d b=%d\n", a, b);

        std::vector<int> p = A->GetP();
        std::vector<int> i = A->GetI();
        std::vector<double> x = A->GetX();

        int decrement = 0;

        for(int rank = 1 ; rank < world_size ; rank++)
        {
            int m = A->GetM();
            int extra_columns = (rank == world_size - 1) ? b : 0;
            int n = a + extra_columns;
            MPI_Send(&m, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
            MPI_Send(&n, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
            MPI_Send(&decrement, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
            ///printf("xxx processID=%d decrement=%d\n", rank, decrement);
            // Send divided p
            int one = (rank == 1) ? 1 : 0;
            int zero = 0;
            if(b != 0)
                zero = (rank == 1) ? 0 : 1;
            int zero1 = (rank == 1) ? 0 : 1;
            int size = a + one + extra_columns;
            MPI_Send(&size, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
            MPI_Send(&p[a * (rank - 1) + zero1], size, MPI_INT, rank, 0, MPI_COMM_WORLD);
            size = a + extra_columns;
            MPI_Send(&size, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
            MPI_Send(&v[a * (rank - 1)], size, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD);
            decrement = p[a * rank + zero];
            ///printf("yy processID=%d p=%d\n", rank, p[a * (rank - 1) + zero1]);
            ///printf("decrement=%d\n", decrement);
            //exit(0);

            // Iterates over A*j
            p = A->GetP();
            i = A->GetI();
            x = A->GetX();
            // Calculates the number of non-zero elements for this block.
            int j1 = a * (rank - 1);
            int j2 = j1 + a - 1 + extra_columns;
            int nnz = 0;
            for(int j = j1 ; j <= j2 ; ++j)
            {
                int i1 = p[j];         // Start index in _i
                int i2 = p[j + 1] - 1; // End index in _i
                nnz += (i2 - i1 + 1);
            }
            ///printf("processID=%d j1=%d nnz=%d j2=%d\n", my_rank, j1, nnz, j2);
            MPI_Send(&nnz, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
            MPI_Send(&i[p[j1]], nnz, MPI_INT, rank, 0, MPI_COMM_WORLD);
            MPI_Send(&x[p[j1]], nnz, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD);
            //exit(0);
        }
    }

    else
    {
        // Receive column blocks of A
        int m, n, nnz, size, decrement;
        std::vector<int> p;
        std::vector<int> i;
        std::vector<double> x;
        std::vector<double> v;
        MPI_Recv(&m, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&decrement, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        ///printf("processID=%d decrement=%d\n", my_rank, decrement);
        MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if(my_rank == 1)
        {
            p = std::vector<int>(size);
            MPI_Recv(&p[0], size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else
        {
            p = std::vector<int>(1 + size);
            p[0] = 0;
            MPI_Recv(&p[1], size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int j = 1 ; j < n + 1 ; ++j)
                p[j] -= decrement;
        }
        MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        v.resize(size);
        MPI_Recv(&v[0], size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&nnz, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        i = std::vector<int>(nnz);
        x = std::vector<double>(nnz);
        MPI_Recv(&i[0], nnz, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&x[0], nnz, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        A_block->m = m; A_block->n = n; A_block->p = p; A_block->i = i; A_block->x = x; A_block->v = v; A_block->decrement = decrement; A_block->nnz = nnz;
        ///print_matrix(A_block, "B_block", my_rank);
        ///print_vector(v, "v", my_rank);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    return 0;
}
//---------------------------------------
int collect_distributed_data(int my_rank, info_load_t *A)
{
    if(my_rank == 0)
    {
        A->r.resize(A->m);
        memset(&A->r[0], 0, A->r.size() * sizeof(double));
    }
    // Reducing on process 0 :
    //MPI_Reduce(&A->r[0], &A->r[0], A->r.size(), MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Allreduce(MPI_IN_PLACE, &A->r[0], A->r.size(), MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    return 0;
}
//---------------------------------------
void perform_block_multiplication_vector(int my_rank, info_load_t *A)
{
    //return;
    A->r.resize(A->m);
    memset(&A->r[0], 0, A->r.size() * sizeof(double));
    //std::vector<double> r = std::vector<double>(5);

    for(int j = 0 ; j < A->n ; ++j)
    {
        // Iterates over A*j
        int i1 = A->p[j];         // Start index in _i
        int i2 = A->p[j + 1] - 1; // End index in _i

        //printf("j=%d i1=%d i2=%d\n", j, i1, i2);

        for(int i = i1 ; i <= i2 ; ++i)
        {
            int iIndex = A->i[i];
            double val = A->x[i];
            A->r[iIndex] += val * A->v[j];
        }
    }
    //sleep(100000);
}
//---------------------------------------
info_load_t *load_matrix()
{
    // -------------- Read n ----------------------
    ifstream n_file("data/n.dat");
    if(!n_file.is_open())
    {
        n_file.close();
        throw std::runtime_error("Could not open n.dat");
    }

    int n = -1;
    n_file >> n;
    n_file.close();
    printf("n=%d\n", n);

    // -------------- Read p ----------------------
    // Allocate p vector
    std::vector<int> p;
    load_vector<int>(p, "data/p.dat");
    printf("p.size=%d\n", p.size());
    // -------------- Read i ----------------------
    // Allocate i vector
    std::vector<int> i;
    load_vector<int>(i, "data/i.dat");
    printf("i.size=%d\n", i.size());
    // -------------- Read x ----------------------
    // Allocate x vector
    std::vector<double> x;
    load_vector<double>(x, "data/x.dat");
    printf("x.size=%d\n", x.size());
    // -------------- Read v ----------------------
    // Allocate x vector
    std::vector<double> v;
    load_vector<double>(v, "data/v.dat");
    printf("v.size=%d\n", v.size());
    // -------------- Create info ----------------------
    info_load_t *mat = new info_load_t;
    mat->m = n;
    mat->n = n;
    mat->p = p;
    mat->i = i;
    mat->x = x;
    mat->v = v;

    return mat;
}
//---------------------------------------
void sequential_multiplication()
{
    info_load_t *mat = load_matrix();
    SparseMatrix *A = new SparseMatrix(mat->m, mat->n, mat->p, mat->i, mat->x);

    /*SparseMatrix *A = new SparseMatrix(5, 5);
    A->Set(0, 0, 1);
    A->Set(1, 2, -1);
    A->Set(2, 0, 2);
    A->Set(2, 2, 1);
    A->Set(3, 1, -1);
    A->Set(4, 4, 3); //

    std::vector<double> v = std::vector<double>(5);
    v[0] = 1;
    v[1] = 2;
    v[2] = 3;
    v[3] = 4;
    v[4] = 5;

    std::cout << *A << std::endl << std::flush;
    print_vector(v, "v", 0);*/

    std::vector<double> r;

    ///A->Print();

    //x[0] = b[0] / L[0];
    SparseMatrix::MultiplyByVector(A, mat->v, r);

    //print_vector(r, "r", 0);

    delete A;
    delete mat;
}
//
//---------------------------------------
void print_execution_time(struct timeval &start, struct timeval &stop)
{
    double d1, d2;
	d1 = (double)start.tv_sec + 1e-6*((double)start.tv_usec);
	d2 = (double)stop.tv_sec + 1e-6*((double)stop.tv_usec);
	// return result in seconds
	double totalSimulationTime = d2 - d1;

	cout << "Multiplication completed in " << totalSimulationTime << " secs." << endl;
}
//---------------------------------------
template <typename T>
void load_vector(std::vector<T> &v, const std::string &filename)
{
    std::string line;
    ifstream file(filename);
    if(!file.is_open())
    {
        file.close();
        throw std::runtime_error("Could not open " + filename);
    }

    while(getline(file, line) )
    {
        T val;
        if(typeid(T) == typeid(int))
            val = std::stoi(line);
        else if(typeid(T) == typeid(double))
            val = std::stod(line);
        else
        {
            file.close();
            throw std::runtime_error("T is not supported");
        }

        //cout << value << '\n';
        v.push_back(val);
    }

    file.close();
}
//---------------------------------------
void print_matrix(info_load_t *matrix, const std::string &name, int my_rank)
{
    std::cout << name << " processID=" << my_rank << "\n" << "p";

    for(std::vector<int>::iterator it = matrix->p.begin() ; it != matrix->p.end() ; ++it)
    {
        int val = *it;
        std::cout << " " << val;
    }
    std::cout << std::endl;

    std::cout << "i";
    for(std::vector<int>::iterator it = matrix->i.begin() ; it != matrix->i.end() ; ++it)
    {
        int val = *it;
        std::cout << " " << val;
    }
    std::cout << std::endl;

    std::cout << "x";
    for(std::vector<double>::iterator it = matrix->x.begin() ; it != matrix->x.end() ; ++it)
    {
        double val = *it;
        std::cout << " " << val;
    }

    std::cout << std::endl;

    std::cout << "-------------" << std::endl;
}
//---------------------------------------
void print_vector(const std::vector<double> &b, const std::string &name, int my_rank)
{
    std::cout << name << " processID=" << my_rank << "\n";

    for(auto it = b.begin() ; it != b.end() ; ++it)
    {
        double val = *it;
        std::cout << " " << val;
    }
    std::cout << std::endl;
}
//---------------------------------------

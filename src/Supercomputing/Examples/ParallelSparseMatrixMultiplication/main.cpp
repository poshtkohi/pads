/**
    #define meta ...
    printf("%s\n", meta);
**/

/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
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
};
//---------------------------------------
int setup_mpi_environment();
int distribute_data(int, info_load_t *, info_load_t *);
int collect_distributed_data(int, info_load_t *, info_load_t **);
info_load_t *perform_block_multiplication(int, info_load_t *, info_load_t *);
void cleanup_mpi_environment(info_load_t *, info_load_t *, info_load_t *);
info_load_t *load_matrix();
template <typename T>
void load_vector(std::vector<T> &, const std::string &);
void sequential_multiplication();
void print_execution_time(struct timeval &, struct timeval &);
void print_matrix(info_load_t *, const std::string &, int);
//---------------------------------------
int main(int argc, char **argv)
{
    info_load_t *A = new info_load_t;
    info_load_t *B_block = new info_load_t;
    info_load_t *C_block = NULL;
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
    if(distribute_data(my_rank, A, B_block) == -1)
        goto Cleanup;

    if(my_rank > 0)
    {
        C_block = perform_block_multiplication(my_rank, A, B_block);
        //goto Cleanup;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    collect_distributed_data(my_rank, A, &C_block);

    if(my_rank == 0)
    {
        //printf("xxxxxx %d\n", C_block->p.size());
        ///SparseMatrix *C = new SparseMatrix(C_block->m, C_block->n, C_block->p, C_block->i, C_block->x);
        ///std::cout << *C << std::endl << std::flush;
        ///delete C;
    }

Cleanup:

    if(my_rank == 0 || my_rank == -2)
    {
        gettimeofday(&stop, NULL);
        print_execution_time(start, stop);
    }

    cleanup_mpi_environment(A, B_block, C_block);

    return 0;
    //sequential_multiplication();
    //return 0;
}
//---------------------------------------
// An example how to work with the sequential matrix sparse library
/*int main(int argc, char **argv)
{
    SparseMatrix *A = new SparseMatrix(4, 4);
    A->Set(0, 0, 1);
    A->Set(1, 2, -1);
    A->Set(2, 0, 2);
    A->Set(2, 2, 1);
    A->Set(3, 1, -1);

    //A->Set(0, 1, 100);
    //A->Set(1, 2, 0);

    SparseMatrix *B = new SparseMatrix(4, 4);
    B->Set(0, 3, 2);
    B->Set(1, 2, 2);
    B->Set(2, 0, 1);
    B->Set(2, 1, 1);
    B->Set(2, 2, -1);
    B->Set(3, 0, 1);

    std::cout << *A << std::endl << std::flush;
    std::cout << *B << std::endl << std::flush;

    A->Print();
    B->Print();

    //SparseMatrix *C = SparseMatrix::Add(A, B);
    //C->EliminateZeros();
    //SparseMatrix *C = SparseMatrix::Transpose(A);
    SparseMatrix *C = SparseMatrix::Multiply(A, B);

    //std::cout << A->Get(2, 2) << std::endl << std::flush;
    std::cout << "C" << std::endl << std::flush;
    std::cout << *C << std::endl << std::flush;
    C->Print();

    delete A;
    delete B;
    delete C;

    return 0;
}*/
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
        cleanup_mpi_environment(NULL, NULL, NULL);
        return -1;
    }

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    return world_rank;
}
//---------------------------------------
void cleanup_mpi_environment(info_load_t *A, info_load_t *B_block, info_load_t *C_block)
{
    if(A != NULL)
        delete A;

    if(B_block != NULL)
        delete B_block;

    if(C_block != NULL)
        delete C_block;

    MPI_Finalize();
}
//---------------------------------------
int distribute_data(int my_rank, info_load_t *A, info_load_t *B_block)
{
    if(A == NULL || B_block == NULL)
    {
        const std::string err_str = "A or B_block could not be null";
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
        SparseMatrix *AA = new SparseMatrix(mat->m, mat->n, mat->p, mat->i, mat->x);
        SparseMatrix *B = new SparseMatrix(mat->m, mat->n, mat->p, mat->i, mat->x);

        /*
        //SparseMatrix *AA = new SparseMatrix(4, 4);
        SparseMatrix *AA = new SparseMatrix(5, 5);
        AA->Set(0, 0, 1);
        AA->Set(1, 2, -1);
        AA->Set(2, 0, 2);
        AA->Set(2, 2, 1);
        AA->Set(3, 1, -1);
        AA->Set(4, 4, 3); //



        //SparseMatrix *B = new SparseMatrix(4, 4);
        SparseMatrix *B = new SparseMatrix(5, 5);
        B->Set(0, 3, 2);
        B->Set(1, 2, 2);
        B->Set(2, 0, 1);
        B->Set(2, 1, 1);
        B->Set(2, 2, -1);
        B->Set(3, 0, 1);
        B->Set(4, 4, 4); //

        std::cout << *B << std::endl << std::flush;
        */

        A->m = AA->GetM(); A->n = AA->GetN(); A->p = AA->GetP(); A->i = AA->GetI(); A->x = AA->GetX();

        if(AA->GetM() != AA->GetN() || AA->GetM() != B->GetM() || AA->GetM() != B->GetN())
        {
            delete AA;
            delete B;
            ///delete mat;
            const std::string err_str = "The number of rows and columns of the two matrices must be equal.";
            //printf("%s\n", err_str.c_str());
            throw std::range_error(err_str);
            //cleanup_mpi_environment();
            return -1;
        }

        if(AA->GetN() < world_size - 1)
        {
            delete AA;
            delete B;
            ///delete mat;
            const std::string err_str = "Matrix dimension is less than the number of MPI processes minus one";
            //printf("%s\n", err_str.c_str());
            throw std::range_error(err_str);
            //cleanup_mpi_environment();
            return -1;
        }

        // Send A
        int m = AA->GetM();
        int n = AA->GetN();
        int nnz = AA->GetNNZ();
        std::vector<int> p = AA->GetP();
        std::vector<int> i = AA->GetI();
        std::vector<double> x = AA->GetX();
        for(int rank = 1 ; rank < world_size ; rank++)
        {
            MPI_Send(&m, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
            MPI_Send(&n, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
            MPI_Send(&p[0], p.size(), MPI_INT, rank, 0, MPI_COMM_WORLD);
            MPI_Send(&nnz, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
            MPI_Send(&i[0], i.size(), MPI_INT, rank, 0, MPI_COMM_WORLD);
            MPI_Send(&x[0], x.size(), MPI_DOUBLE, rank, 0, MPI_COMM_WORLD);
        }
        p.clear();
        i.clear();
        x.clear();

        // Send column blocks of B
        int a = B->GetN() / (world_size - 1);
        int b = B->GetN() % (world_size - 1);

        ///printf("a=%d b=%d\n", a, b);

        int to_prev = 0;

        p = B->GetP();
        i = B->GetI();
        x = B->GetX();

        /*
        std::cout << "p";

        for(std::vector<int>::iterator it = p.begin() ; it != p.end() ; ++it)
        {
            int val = *it;
            std::cout << " " << val;
        }
        std::cout << std::endl;
        */
        //exit(0);
        int decrement = 0;

        for(int rank = 1 ; rank < world_size ; rank++)
        {
            int to_next = a * rank - 1; //
            int m = B->GetM();
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
            decrement = p[a * rank + zero];
            ///printf("yy processID=%d p=%d\n", rank, p[a * (rank - 1) + zero1]);
            ///printf("decrement=%d\n", decrement);
            //exit(0);

            // Iterates over B*j
            p = B->GetP();
            i = B->GetI();
            x = B->GetX();
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

            // Iterates over B*j
            //int i1B = (*B->_p)[jC];         // Start index in _i
            //int i2B = (*B->_p)[jC + 1] - 1; // End index in _i
            to_prev = to_next;
        }
    }

    else
    {
        // Receive A
        int m, n, nnz;
        MPI_Recv(&m, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::vector<int> p = std::vector<int>(n + 1);
        MPI_Recv(&p[0], p.size(), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&nnz, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::vector<int> i = std::vector<int>(nnz);
        std::vector<double> x = std::vector<double>(nnz);
        MPI_Recv(&i[0], i.size(), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&x[0], x.size(), MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
///        SparseMatrix *AA = new SparseMatrix(m, n, p, i, x);
///        std::cout << *AA << std::endl << std::flush;
///        delete AA;
        //exit(0);
        A->m = m; A->n = n; A->p = p; A->i = i; A->x = x; A->nnz = nnz;

        // Receive column blocks of B
        int size, decrement;
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
        MPI_Recv(&nnz, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        i = std::vector<int>(nnz);
        x = std::vector<double>(nnz);
        MPI_Recv(&i[0], nnz, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&x[0], nnz, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        B_block->m = m; B_block->n = n; B_block->p = p; B_block->i = i; B_block->x = x; B_block->decrement = decrement; B_block->nnz = nnz;
        ///print_matrix(B_block, "B_block", my_rank);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    return 0;
}
//---------------------------------------
int collect_distributed_data(int my_rank, info_load_t *A, info_load_t **C_block)
{
    if(my_rank == 0)
    {
        *C_block = new info_load_t;
        info_load_t *C = *C_block;
        C->m = A->m; C->n = A->m;
        C->p.resize(1);

        // Get the number of processes
        int world_size;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);

        // First we collect i and x from all processes.
        int increment = 0;
        for(int rank = 1 ; rank < world_size ; ++rank)
        {
            // Receive p
            int size, new_size = 0;
            MPI_Recv(&size, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            ///printf("rank=%d size=%d\n", rank, size);
            new_size = C->p.size() + size;
            C->p.resize(new_size);
            int index = new_size - size;
            MPI_Recv(&C->p[index], size, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int j = index, k = 1 ; k <= size ; ++j, ++k)
                C->p[j] += increment;
            increment = C->p[index + size - 1];
            // Receive i and x
            int nnz;
            MPI_Recv(&nnz, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            C->nnz = nnz;
            ///printf("nnz=%d\n", nnz);
            new_size = C->i.size() + nnz;
            C->i.resize(new_size);
            C->x.resize(new_size);
            ///printf("rank=%d size-nnz=%d\n", rank, new_size - nnz);
            MPI_Recv(&C->i[new_size - nnz], nnz, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&C->x[new_size - nnz], nnz, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            ///print_matrix(C, "C", my_rank);
        }
    }
    else
    {
        // First we collect i and x from all processes.
        info_load_t *Cb = *C_block;
        int size = Cb->p.size() - 1;
        MPI_Send(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&Cb->p[1], size, MPI_INT, 0, 0, MPI_COMM_WORLD);
        // Send i and x
        MPI_Send(&Cb->nnz, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&Cb->i[0], Cb->i.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&Cb->x[0], Cb->x.size(), MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    return 0;
}
//---------------------------------------
info_load_t *perform_block_multiplication(int my_rank, info_load_t *A, info_load_t *B_block)
{
    info_load_t *C_block = new info_load_t;

    /*for(int j = 0 ; j < B_block->n ; ++j)
    {
        // Iterates over B*j
        //int decrement1 = (my_rank > 1 && j > 0) ? B_block->decrement : 0;
        //int decrement2 = (my_rank > 1) ? B_block->decrement : 0;
        int i1B = B_block->p[j];          // Start index in i
        int i2B = B_block->p[j + 1] - 1;  // End index in _i
        printf("processID=%d j=%d p[%d]=%d i1B=%d i2B=%d\n", my_rank, j, j, B_block->p[j], i1B, i2B);
        for(int iB = i1B ; iB <= i2B ; ++iB)
        {
            int iIndexB = B_block->i[iB];
            double valB = B_block->x[iB];
            printf("processID=%d iIndexB=%d j=%d\n", my_rank, iIndexB, j);
        }
    }
    return C_block;*/
    C_block->m = A->m; C_block->n = B_block->n; C_block->nzmax = A->nnz + B_block->nnz;
    C_block->p.resize(C_block->n + 1);
    C_block->i.resize(C_block->nzmax);
    C_block->x.resize(C_block->nzmax);
    int num_of_rows = C_block->m;
    int num_of_columns = C_block->n;
    std::vector<int> w(num_of_rows);     // workspace
    std::vector<double> x(num_of_rows);  // dense vector workspace
    int nz_total = 0;

    ///printf("processID=%d m=%d n=%d\n", my_rank, C_block->m, C_block->n);
    //return C_block;

    for(int jC = 0 ; jC < num_of_columns ; ++jC)
    {
        int nz = 0;

        // Iterates over B*j
        int i1B = B_block->p[jC];          // Start index in i
        int i2B = B_block->p[jC + 1] - 1;  // End index in _i
        ///printf("processID=%d jC=%d i1B=%d i2B=%d decrement=%d\n", my_rank, jC, i1B, i2B, B_block->decrement);

        for(int iB = i1B ; iB <= i2B ; ++iB)
        {
            int iIndexB = B_block->i[iB];
            double valB = B_block->x[iB];
            ///printf("processID=%d iIndexB=%d jC=%d\n", my_rank, iIndexB, jC);

            // Iterates over A*j
            int i1A = A->p[iIndexB];            // Start index in _i
            int i2A = A->p[iIndexB + 1] - 1;    // End index in _i

            for(int iA = i1A ; iA <= i2A ; ++iA)
            {
                int iIndexA = A->i[iA];
                double valA = A->x[iA];

                ///printf("processID=%d iIndexA=%d jC=%d\n", my_rank, iIndexA, jC);
                if(w[iIndexA] == jC + 1) // it means that the item exists in w
                {
                    x[iIndexA] += valA * valB;
                }
                else
                {
                    // Allocate space
                    if(C_block->nzmax < nz_total + 1)
                    {
                        ///printf("nzmax=%d nz_total=%d\n", C_block->nzmax, nz_total + 1);
                        C_block->nzmax = (nz_total + 1) * 2;
                        C_block->i.resize(C_block->nzmax);
                        C_block->x.resize(C_block->nzmax);
                    }
                    w[iIndexA] = jC + 1;
                    //C->_i->push_back(iIndex);
                    C_block->i[nz_total] = iIndexA;
                    //_x->push_back(val);
                    x[iIndexA] = valA * valB;
                    nz++;
                    nz_total++;
                }
            }
        }

        for(int i = nz_total - nz, k = 1 ; k <= nz ; ++i, ++k)
        {
            int iIndex = C_block->i[i];
            double val = x[iIndex];
            C_block->i[i] = iIndex;
            C_block->x[i] = val;
            //printf("here2 i=%d iIndex=%d val=%g\n", i, iIndex, val);
        }
        //nz_total += nz;
        C_block->p[jC + 1] = nz_total;
        //break;
    }

    // Cleanup C_block
    C_block->i.resize(nz_total);
    C_block->x.resize(nz_total);
    C_block->nnz = nz_total;
    C_block->nzmax = nz_total;

    ///print_matrix(C_block, "C_block", my_rank);

    return C_block;
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

    // -------------- Create info ----------------------
    info_load_t *mat = new info_load_t;
    mat->m = n;
    mat->n = n;
    mat->p = p;
    mat->i = i;
    mat->x = x;

    return mat;
}
//---------------------------------------
void sequential_multiplication()
{
    info_load_t *mat = load_matrix();

    SparseMatrix *A = new SparseMatrix(mat->m, mat->n, mat->p, mat->i, mat->x);
    SparseMatrix *B = new SparseMatrix(mat->m, mat->n, mat->p, mat->i, mat->x);

    /*
    //SparseMatrix *A = new SparseMatrix(4, 4);
    SparseMatrix *A = new SparseMatrix(5, 5);
    A->Set(0, 0, 1);
    A->Set(1, 2, -1);
    A->Set(2, 0, 2);
    A->Set(2, 2, 1);
    A->Set(3, 1, -1);
    A->Set(4, 4, 3); //



    //SparseMatrix *B = new SparseMatrix(4, 4);
    SparseMatrix *B = new SparseMatrix(5, 5);
    B->Set(0, 3, 2);
    B->Set(1, 2, 2);
    B->Set(2, 0, 1);
    B->Set(2, 1, 1);
    B->Set(2, 2, -1);
    B->Set(3, 0, 1);
    B->Set(4, 4, 4); //
    */

    ///std::cout << *A << std::endl << std::flush;
    ///std::cout << *B << std::endl << std::flush;

    ///A->Print();
    ///B->Print();

    ///struct timeval start, stop;
    ///gettimeofday(&start, NULL);

    //x[0] = b[0] / L[0];
    SparseMatrix *C = SparseMatrix::Multiply(A, B);

    ///gettimeofday(&stop, NULL);

    ///std::cout << *C << std::endl << std::flush; //
    ///C->Print();

    //delete mat;
    delete A;
    delete B;
    delete C;

    ///print_execution_time(start, stop);
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

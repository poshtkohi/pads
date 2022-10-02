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
#include <math.h>

#include "balanced_binary_tree.h"

using namespace std;
using namespace Supercomputing::CollectiveOperations;

#define MPI_DATA_TAG 729
//---------------------------------------
void print_execution_time(struct timeval &, struct timeval &, const std::string &);
void test_mpi_broadcast(int , char **);
void test_blocking_broadcast(int , char **);
void test_broadcast_balanced_binary_tree(int , char **);
//---------------------------------------
int main(int argc, char **argv)
{
    //test_mpi_broadcast(argc, argv);
    //test_blocking_broadcast(argc, argv);
    test_broadcast_balanced_binary_tree(argc, argv);
    return 0;
}
//---------------------------------------
void test_mpi_broadcast(int argc, char **argv)
{
    int num_procs, rank;
    //int global_sum;
    struct timeval start, stop;

    double t = 0.0;
    const double time_step = 10.0e-6;
    const double simulation_until = 1.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(num_procs <= 2)
        throw std::runtime_error("Number of MPI processes must be greater than 2");

    //printf("I'm the process %d\n", rank);
    //MPI_Barrier(MPI_COMM_WORLD);

    int root = 0;//num_procs - 1;

    if(rank == root)
        gettimeofday(&start, NULL);

    while(t <= simulation_until)
    {
        // Controller process.
        if(rank == root)
        {
            t += time_step;
            /* everyone calls bcast, data is taken from root and ends up in everyone's buf */
            MPI_Bcast(&t, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
            //MPI_Bcast(&_tt, sizeof(tt), MPI_BYTE, root, MPI_COMM_WORLD);
            //MPI_Reduce(&rank, &global_sum, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Bcast(&t, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
            //MPI_Reduce(&rank, &global_sum, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    //printf("t %g in process rank %d\n", t, rank);

    if(rank == root)
    {
        gettimeofday(&stop, NULL);
        print_execution_time(start, stop, "test_mpi_broadcast");
    }

    MPI_Finalize();
}
//----------------------------------------------------
void test_blocking_broadcast(int argc, char **argv)
{
    int num_procs, rank;
    struct timeval start, stop;

    double t = 0.0;
    const double time_step = 10.0e-6;
    const double simulation_until = 1.0;

    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(num_procs <= 2)
        throw std::runtime_error("Number of MPI processes must be greater than 2");

    //printf("I'm the process %d\n", rank);
    //MPI_Barrier(MPI_COMM_WORLD);

    int root = 0;//num_procs - 1;

    if(rank == root)
        gettimeofday(&start, NULL);

    while(t <= simulation_until)
    {
        // Controller process.
        if(rank == root)
        {
            t += time_step;

            //for(int i = 0 ; i < num_procs - 1 ; i++)
            for(int i = 1 ; i < num_procs ; i++)
                MPI_Send(&t, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Recv(&t, 1, MPI_DOUBLE, root, 0, MPI_COMM_WORLD, &status);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    //printf("t %g in process rank %d\n", t, rank);

    if(rank == root)
    {
        gettimeofday(&stop, NULL);
        print_execution_time(start, stop, "test_blocking_broadcast");
    }

    MPI_Finalize();
}
//----------------------------------------------------
void test_broadcast_balanced_binary_tree(int argc, char **argv)
{
    int num_procs, rank;
    MPI_Request request;
    MPI_Status status;
    int parent, left, right;
    MPI_Request requests[2];
    MPI_Status statuses[2];
    struct timeval start, stop;

    balanced_binary_tree::nodeType my_node;
    double t = 0.0;
    const double time_step = 10.0e-6;
    const double simulation_until = 1.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(num_procs <= 2)
        throw std::runtime_error("Number of MPI processes must be greater than 2");

    //printf("I'm the process %d\n", rank);
    //MPI_Barrier(MPI_COMM_WORLD);

    int root = 0;//num_procs - 1;

    // Controller process.
    if(rank == root)
    {
        // Distributes BST information across all MPI processes.
        std::vector<unsigned int> *process_ranks = new std::vector<unsigned int>;
        for(int i = 0 ; i < num_procs ; i++)
            process_ranks->push_back(i);

        balanced_binary_tree tree(*process_ranks, root);
        std::vector<balanced_binary_tree::nodeType> &nodes = tree.get_nodes();
        for(std::vector<balanced_binary_tree::nodeType>::iterator it = nodes.begin() ; it != nodes.end() ; ++it)
        {
            auto node = *it;
            int dest_node = std::get<0>(node);
            if(dest_node == root)
                my_node = node;
            else if(dest_node != -1)
                MPI_Send(&node, sizeof(balanced_binary_tree::nodeType), MPI_BYTE, dest_node, MPI_DATA_TAG, MPI_COMM_WORLD);

            //printf("NodeID=%d NodeParent=%d NodeLeft=%d NodeRight=%d\n", std::get<0>(node), std::get<1>(node), std::get<2>(node), std::get<3>(node));
            //printf("Controller: node %d parent %d left %d right %d\n", std::get<0>(node), std::get<1>(node), std::get<2>(node), std::get<3>(node));
        }

        //exit(0);
        delete process_ranks;

    }
    else
        // Receives BST node information from controller.
        MPI_Recv(&my_node, sizeof(balanced_binary_tree::nodeType), MPI_BYTE, root, MPI_DATA_TAG, MPI_COMM_WORLD, &status);

    parent = std::get<1>(my_node);
    left = std::get<2>(my_node);
    right = std::get<3>(my_node);

    //printf("node %d parent %d left %d right %d\n", std::get<0>(my_node), std::get<1>(my_node), std::get<2>(my_node), std::get<3>(my_node));

    if(rank == root)
        gettimeofday(&start, NULL);

    // Begins processing.
    while(t < simulation_until)
    {
        // Controller process.
        if(rank == root)
        {
            t += time_step;

            // Broadcasts t.
            if(left != -1)
                MPI_Isend(&t, 1, MPI_DOUBLE, left, MPI_DATA_TAG, MPI_COMM_WORLD, &requests[0]);
            if(right != -1)
                MPI_Isend(&t, 1, MPI_DOUBLE, right, MPI_DATA_TAG, MPI_COMM_WORLD, &requests[1]);

            if(left != -1 && right != -1)
                MPI_Waitall(2, requests, statuses);
            else if(left != -1)
                MPI_Wait(&requests[0], &statuses[0]);
            else if(right != -1)
                MPI_Wait(&requests[1], &statuses[1]);
        }
        else
        {
            // Receives the data and broadcasts data to tis children.
            if(parent != -1)
            {
                MPI_Irecv(&t, 1, MPI_DOUBLE, parent, MPI_DATA_TAG, MPI_COMM_WORLD, &request);
                MPI_Wait(&request, &status);
            }
            else
                continue;

            if(left != -1)
                MPI_Isend(&t, 1, MPI_DOUBLE, left, MPI_DATA_TAG, MPI_COMM_WORLD, &requests[0]);
            if(right != -1)
                MPI_Isend(&t, 1, MPI_DOUBLE, right, MPI_DATA_TAG, MPI_COMM_WORLD, &requests[1]);

            if(left != -1 && right != -1)
                MPI_Waitall(2, requests, statuses);
            else if(left != -1)
                MPI_Wait(&requests[0], &statuses[0]);
            else if(right != -1)
                MPI_Wait(&requests[1], &statuses[1]);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == root)
    {
        gettimeofday(&stop, NULL);
        print_execution_time(start, stop, "test_broadcast_balanced_binary_tree");
    }

    //MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
}
//---------------------------------------
void print_execution_time(struct timeval &start, struct timeval &stop, const std::string &str)
{
    double d1, d2;
	d1 = (double)start.tv_sec + 1e-6*((double)start.tv_usec);
	d2 = (double)stop.tv_sec + 1e-6*((double)stop.tv_usec);
	// return result in seconds
	double totalSimulationTime = d2 - d1;

	std::cout << str << " completed in " << totalSimulationTime << " secs." << endl;
}
//---------------------------------------

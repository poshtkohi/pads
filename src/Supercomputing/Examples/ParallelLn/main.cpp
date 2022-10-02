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

using namespace std;
//---------------------------------------
struct task_info
{
    long double x0;
    long double x1;
    long double dx;
};
//---------------------------------------
int setup_mpi_environment();
void cleanup_mpi_environment(task_info *);
void distribute_data(int, task_info *);
long double collect_distributed_data(int, long double local_sum);
long double perform_ln(long double, long double, const long double);
void sequential_ln(task_info *);
void print_execution_time(struct timeval &, struct timeval &);
//---------------------------------------
int main(int argc, char **argv)
{
    task_info *task = new task_info;
    long double local_sum = 0.0;
    long double global_sum;

    struct timeval start, stop;

    int my_rank = setup_mpi_environment();

    if(my_rank == 0 || my_rank == -2)
        gettimeofday(&start, NULL);

    // Initialises the integration parameters used by the controller process.
    if(my_rank == 0 || my_rank == -2)
    {
        long double x = 2.0;
        task->x0 = 1.0;
        task->x1 = x;
        task->dx = 1e-9;
    }

    if(my_rank == -1)
        goto Cleanup;
    else if (my_rank == -2)
    {
        sequential_ln(task);
        goto Cleanup;
    }

    //printf("my_rank=%d\n", my_rank);
    distribute_data(my_rank, task);

    printf("my_rank=%d x0=%Lg x1=%Lg dx=%Lg\n", my_rank, task->x0, task->x1, task->dx);

    if(my_rank > 0)
    {
        //printf("%Lg\n", task->dx);
        local_sum = perform_ln(task->x0, task->x1, task->dx);
        //goto Cleanup;
    }

    //MPI_Barrier(MPI_COMM_WORLD);

    printf("my_rank=%d local_sum=%Lg\n", my_rank, local_sum);

    global_sum = collect_distributed_data(my_rank, local_sum);

    if(my_rank == 0)
    {
        printf("my_rank=%d ln(%Lg)=%.20Lg\n", my_rank, task->x1, global_sum);
    }

Cleanup:

    if(my_rank == 0 || my_rank == -2)
    {
        gettimeofday(&stop, NULL);
        print_execution_time(start, stop);
    }

    cleanup_mpi_environment(task);

    return 0;
    //sequential_multiplication();
    //return 0;
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
void cleanup_mpi_environment(task_info *task)
{
    //return;
    //if(task != NULL)
    //    delete task;

    MPI_Finalize();
}
//---------------------------------------
void distribute_data(int my_rank, task_info *task)
{
    if(my_rank == 0) // Controller process
    {
        // Get the number of processes
        int world_size;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);

        int useful_workers = world_size - 1;
        std::vector<long double> works(3 * useful_workers + 3);

        long double x0 = task->x0;
        long double x1 = task->x1;
        long double dx = task->dx;
        long double current_x = x0;
        long double step_x = (x1 - x0) / (long double)useful_workers;

        ///printf("my_rank=%d current_x=%Lg step_x=%Lg size=%d\n", my_rank, current_x, step_x, works.size());

        for(int i = 3 ; i < works.size() ; i += 3)
        {
            works[i] = current_x;     // x0 in i'th process
            current_x += step_x;
            works[i + 1] = current_x; // x1 in i'th process
            works[i + 2] = dx;        // dx in i'th process
        }

        for(int i = 0 ; i < works.size() ; i++)
        {
            long double val = works[i];
            std::cout << " " << val;
        }
        std::cout << std::endl;

        MPI_Scatter(&works[0], 3, MPI_LONG_DOUBLE, NULL, 0, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
    }
    else // Worker process
    {
        std::vector<long double> my_work(3); // Contains x0 and x1 for the current process.

        MPI_Scatter(NULL, 0, MPI_LONG_DOUBLE, &my_work[0], 3, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);

        long double x0 = my_work[0];
        long double x1 = my_work[1];
        long double dx = my_work[2];

        ///printf("my_rank=%d x0=%Lg x1=%Lg dx=%Lg\n", my_rank, x0, x1, dx);

        task->x0 = x0;
        task->x1 = x1;
        task->dx = dx;
    }

    MPI_Barrier(MPI_COMM_WORLD);
}
//---------------------------------------
long double collect_distributed_data(int my_rank, long double local_sum)
{
    long double global_sum;

    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    return global_sum;
}
//---------------------------------------
long double perform_ln(long double x0, long double x1, const long double dx)
{
    // Trapezoidal integration rule
    // https://en.wikipedia.org/wiki/Trapezoidal_rule
    long double sum = 1.0 / x0 + 1.0 / x1;
    long double x = x0;

    while(x < x1)
    {
        x += dx;
        sum += 2.0 / x;
    }

    sum *= dx / 2.0;

    // Simpson's integration rule
    // https://en.wikipedia.org/wiki/Simpson%27s_rule
    /*long double sum = 1.0 / x0 + 1.0 / x1;
    long double x = x0;
    long double i = 4.0;

    while(x < x1)
    {
        x += dx;
        sum += i * (1.0 / x);
        if(i == 2.0)
            i = 4.0;
        else
            i = 2.0;
    }

    sum *= dx / 3.0;*/

    return sum;
}
//---------------------------------------
void sequential_ln(task_info *task)
{
    long double ln = perform_ln(task->x0, task->x1, task->dx);
    printf("ln(%Lg)=%.20Lg\n", task->x1, ln);
    cout.precision(17);
    cout << "ln " << ln << endl;
    printf("dif=%.20Lg\n", abs(log(task->x1) - ln));
}
//---------------------------------------
void print_execution_time(struct timeval &start, struct timeval &stop)
{
    double d1, d2;
	d1 = (double)start.tv_sec + 1e-6*((double)start.tv_usec);
	d2 = (double)stop.tv_sec + 1e-6*((double)stop.tv_usec);
	// return result in seconds
	double totalSimulationTime = d2 - d1;

	std::cout << "Ln completed in " << totalSimulationTime << " secs." << endl;
}
//---------------------------------------

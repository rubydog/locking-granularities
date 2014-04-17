#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define MAXGRIDSIZE 10
#define MAXTHREADS 1000
#define NO_SWAPS 20

extern int errno;

typedef enum {GRID, ROW, CELL, NONE} grain_type;
int gridsize = 0;
int grid[MAXGRIDSIZE][MAXGRIDSIZE];
int threads_left;
pthread_mutex_t lock;

time_t start_t, end_t;

void print_grid(int grid[MAXGRIDSIZE][MAXGRIDSIZE], int gridsize)
{
	int i, j;

	for (i = 0; i < gridsize; i++)
	{
		for (j = 0; j < gridsize; j++)
			fprintf(stdout, "%d\t", grid[i][j]);
		fprintf(stdout, "\n");
	}
}

void initialize_grid(int grid[MAXGRIDSIZE][MAXGRIDSIZE], int gridsize)
{
	int i, j;

	for (i = 0; i < gridsize; i++)
		for (j = 0; j < gridsize; j++)
			grid[i][j] = rand() % 100;
}

long sum_grid(int grid[MAXGRIDSIZE][MAXGRIDSIZE], int gridsize)
{
	int i, j;
	long sum = 0;

	for (i = 0; i < gridsize; i++)
		for (j = 0; j < gridsize; j++)
			sum += grid[i][j];

	return sum;
}

void error(char* msg)
{
	perror(msg);
	exit(1);
}

void* do_swaps(void *args)
{
	int i, row1, row2, column1, column2, temp;
	grain_type* gain_type = (grain_type *)args;
	threads_left++;

	for (i = 0; i < NO_SWAPS; i++)
	{
		row1 = rand() % gridsize;
		column1 = rand() % gridsize;
		row2 = rand() % gridsize;
		column2 = rand() % gridsize;

		if (*gain_type == GRID)
		{
			pthread_mutex_lock(&lock);
			grid;
		}
		else if (*gain_type == ROW)
		{
			pthread_mutex_lock(&lock);
			grid[row1];
			grid[row2];
		}
		else if (*gain_type == CELL)
		{
			pthread_mutex_lock(&lock);
		}

		temp = grid[row1][column1];
		sleep(1);
		grid[row1][column1] = grid[row2][column2];
		grid[row2][column2] = temp;

		// Release lock
		if (*gain_type == GRID)
		{
			pthread_mutex_unlock(&lock);
		}
		else if (*gain_type == ROW)
		{
			pthread_mutex_unlock(&lock);
		}
		else if (*gain_type == CELL)
		{
			pthread_mutex_unlock(&lock);
		}
	}

	threads_left--;
	if (threads_left == 0)
	{
		time(&end_t);
	}
	return NULL;
}

int main(int argc, char** argv)
{
	int nthreads = 0;
	pthread_t threads[MAXTHREADS];
	grain_type row_granularity = NONE;
	long init_sum = 0;
	long final_sum = 0;
	int i;

	if (argc > 3)
	{
		gridsize = atoi(argv[1]);
		if (gridsize > MAXGRIDSIZE || gridsize < 1)
			error("Grid size must be between 1 to 10\n");

		nthreads = atoi(argv[2]);
		if (nthreads > MAXTHREADS || nthreads < 1)
			error("Number of threads must be between 1 to 1000\n");

		if (argv[3][0] == 'g' || argv[3][0] =='G')
			row_granularity = GRID;
		else if (argv[3][0] == 'r' || argv[3][0] == 'R')
			row_granularity = ROW;
		else if (argv[3][0] == 'c' || argv[3][0] == 'C')
			row_granularity = CELL;
	}
	else
		error("Invalid number of arguments");

	printf("Initial Grid:\n\n");
	initialize_grid(grid, gridsize);
	init_sum = sum_grid(grid, gridsize);
	print_grid(grid, gridsize);
	printf("\nInitial Sum:  %lu\n", init_sum);
	printf("Executing threads...\n");

	srand((unsigned int)time( NULL ));

	pthread_mutex_init(&lock, NULL);
	time(&start_t);
	for (i = 0; i < nthreads; i++)
	{
		if (pthread_create(&(threads[i]), NULL, do_swaps,
				     (void *)(&row_granularity)) != 0)
			error("ERROR thread creation failed");
	}

	for (i = 0; i < nthreads; i++)
		pthread_detach(threads[i]);

	pthread_mutex_destroy(&lock);

	while(1)
	{
		sleep(2);
		if (threads_left == 0)
		{
			fprintf(stdout, "\nFinal Grid:\n\n");
			print_grid(grid, gridsize);
			final_sum = sum_grid(grid, gridsize);
			fprintf(stdout, "\n\nFinal Sum: %lu\n", final_sum);
			fprintf(stdout, "Secs elapsed: %g\n", difftime(end_t, 
								      start_t));
			exit(0);
		}
	}

	return 0;
}

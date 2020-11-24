/* STAGE3.C
 *
 * Author: James Burling
 * Date: 12/11/2020
 * Last Modified: 12/11/2020
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

/* Structure: Workspace
 * Holds all variables required for each worker thread.
 *
 * @variable pointCount - Number of points to calculate
 * @variable seed       - A unique seed for each thread that is provided to the rand_r function
 */
typedef struct WorkspaceStruct{
    int pointCount;
    int seed;
    int id;
}Workspace;

// Global Variables : accessed/shared by all threads
double radius = 1;
int verbose = 0;
volatile int circlePoints = 0, available = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;

/*
 * Function: isInCircle
 * ------------------------
 * Calculates whether the coordinate ( x , y ) is within the bounds of the circle with
 * the radius provided.
 * Uses Pythagoras' Theorem: a^2 + b^2 = c^2 to calculate length of point to the
 * center of the circle. Returns boolean c^2 < r^2 where r is the radius of the circle
 *
 * @param x      - The x coordinate of the point to check is in the circle
 * @param y      - The y coordinate of the point to check is in the circle
 *
 * @return int of 1 if coordinate (x,y) is within the circle, otherwise returns 0
 */
int isInCircle(double x, double y){
    return (x*x) + (y*y) < (radius*radius);
}

/*
 * Function: calculateCirclePoints
 * ------------------------
 * Iterates through a large number of random coordinates, counting how many are within the
 * bounds of a circle. It then stores this number in the workspace provided.
 *
 * @param *ws - void pointer to the workspace of the current thread
 *
 * @return void* that will always be NULL if the thread executes properly
 */
void* calculateCirclePoints(void *ws){
    Workspace *workspace = (Workspace*) ws;

    for(int i = 0; i < workspace->pointCount; i++){
        double x = ((double)rand_r(&workspace->seed)*2 / (double)RAND_MAX) - 1; // Random double between -1 and 1
        double y = ((double)rand_r(&workspace->seed)*2 / (double)RAND_MAX) - 1; // Random double between -1 and 1

        if(isInCircle(x, y)){ // If Random Coordinate is inside circle area
            pthread_mutex_lock(&mutex); // Locks the mutex

            while(available == 0){ // If another thread is updating circle points then wait
                if(verbose){printf("Thread %d - WAITING\n", workspace->id);}
                pthread_cond_wait(&condvar, &mutex); // Wait until signalled
            }

            // Change variables protected by mutex
            available = 0;
            circlePoints++;

            if(verbose){printf("Thread %d - ADDED - Total Circle Points = %d\n", workspace->id, circlePoints);}

            // Unlock mutex allowing other thread to alter circlePoints
            pthread_mutex_unlock(&mutex);
            available = 1;
            // Signal for another thread to start
            pthread_cond_signal(&condvar);
        }
    }

    return NULL;
}

/*
 * Function: calculateCircleArea
 * ------------------------
 * Creates the number of threads provided as a parameter and evenly distributes each thread
 * a number of points to calculate. It waits until all the threads are finished to join
 * them back up and calculate the area of the circle. This value is returned.
 *
 * @param pointCount  - Number of random coordinates to iterate through. The greater the
 *                      pointCount, the more accurate the area calculation will be.
 * @param threadCount - Number of worker threads to create and use to calculate the points.
 *
 * @return double of the calculated area of the circle
 */
double calculateCircleArea(int pointCount, int threadCount){
    Workspace workspaces[threadCount];
    pthread_t workerThreads[threadCount];

    int pointsPerThread = pointCount / threadCount;
    int remainingPoints = pointCount % threadCount;
    int initialSeed = time(NULL);

    for(int i = 0; i < threadCount; i++) {
        workspaces[i].pointCount = pointsPerThread + (i < remainingPoints);
        workspaces[i].seed = initialSeed + i;
        workspaces[i].id = i;

        int status = pthread_create(&(workerThreads[i]), NULL, calculateCirclePoints, &workspaces[i]);
        if(status != 0){
            perror("Error creating Thread: ");
        }
    }

    for(int i = 0; i < threadCount; i++) {
        pthread_join(workerThreads[i], NULL);
    }

    return ((double)circlePoints/(double)pointCount)*4*radius*radius;
}

/*
 * Function: main
 * ------------------------
 * Iterates through a large number of random coordinates, counting how many are within the
 * bounds of a circle. It then calculates a rough estimate for the area of the circle.
 * This process is done with multiple threads, by splitting the total number of point
 * calculations evenly between threads and calculating the sum of the points in the circle
 * once all threads have finished.
 * The time taken to calculate the area of the circle is calculated and displayed on completion.
 *
 * @param argc - Number of command line arguments provided
 * @param *argv[] - array of pointers to the command line arguments
 *        argv[0] - The name of the this executable file.
 *        argv[1...n]:
 *          [-p] number of points to iterate through
 *          [-t] number of worker threads to create
 *          [-r] radius of the circle to calculate
 *          [-c] calculate and display execution time
 *          [-v] print out when each thread add a circle point
 *
 * @return int of how program exits
 */
int main(int argc, char *argv[]) {
    // Default Values, used if not arguments provided
    int pointCount = 100000;
    int threadCount = 10;
    int timer = 0;
    int c;

    // Retrieving Arguments
    while ((c = getopt(argc, argv, "p:t:r:cv")) != -1){
        switch(c){
            case 'p': // Point Count
                pointCount = atoi(optarg);
                break;
            case 't': // Thread Count
                threadCount = atoi(optarg);
                break;
            case 'r': // Radius
                radius = atof(optarg);
                break;
            case 'c': // Clock (timer)
                timer = 1;
                break;
            case 'v': // Verbose (extra information)
                verbose = 1;
                break;
        }
    }


    struct timespec startTime, endTime;
    if(timer) {
        clock_gettime(CLOCK_REALTIME, &startTime); // Get the start time
    }

    double area = calculateCircleArea(pointCount, threadCount); // Calculate the area of the circle

    // Print Results
    printf("Number of Points = %d, Number of Threads = %d, Circle Radius = %f\n", pointCount, threadCount,radius);
    printf("The Area of the circle is: %f\n", area);

    if(timer) {
        clock_gettime(CLOCK_REALTIME, &endTime); // Get the time at the end of the algorithm
        double elapsedTime = (endTime.tv_sec - startTime.tv_sec) + // Calculate the diff in time between start and end
                             (endTime.tv_nsec - startTime.tv_nsec) / 1000000000.0;
        printf("Elapsed Time: %f seconds\n", elapsedTime);
    }

    return EXIT_SUCCESS;
}

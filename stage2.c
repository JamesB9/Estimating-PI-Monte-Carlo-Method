/* STAGE2.C
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
 * @variable pointCount   - Number of points to calculate
 * @variable circlePoints - Number of points calculated that were inside the circle
 * @variable *radius      - Pointer to the double containing the radius of the circle.
 * @variable seed         - A unique seed for each thread that is provided to the rand_r function
 */
typedef struct WorkspaceStruct{
    int pointCount;
    int circlePoints;
    double* radius;
    int seed;
}Workspace;


/*
 * Function: isInCircle
 * ------------------------
 * Calculates whether the coordinate ( x , y ) is within the bounds of the circle with
 * the radius provided.
 * Uses Pythagoras' Theorem: a^2 + b^2 = c^2 to calculate length of point to the
 * center of the circle. Returns boolean c^2 < r^2 where r is the radius of the circle
 *
 * @param radius - Radius of the circle to check.
 * @param x      - The x coordinate of the point to check is in the circle
 * @param y      - The y coordinate of the point to check is in the circle
 *
 * @return int of 1 if coordinate (x,y) is within the circle, otherwise returns 0
 */
int isInCircle(double radius, double x, double y){
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

        if(isInCircle(*workspace->radius, x, y)){
            workspace->circlePoints++;
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
 * @param radius      - Radius of the circle to calculate the area of.
 *
 * @return double of the calculated area of the circle
 */
double calculateCircleArea(int pointCount, int threadCount, double radius){
    Workspace workspaces[threadCount];
    pthread_t workerThreads[threadCount];

    int pointsPerThread = pointCount / threadCount;
    int remainingPoints = pointCount % threadCount;
    int initialSeed = time(NULL);
    int circlePoints = 0;

    for(int i = 0; i < threadCount; i++) {
        workspaces[i].pointCount = pointsPerThread + (i < remainingPoints);
        workspaces[i].circlePoints = 0;
        workspaces[i].radius = &radius;
        workspaces[i].seed = initialSeed + i;

        pthread_create(&(workerThreads[i]), NULL, calculateCirclePoints, &workspaces[i]);
    }

    for(int i = 0; i < threadCount; i++) {
        pthread_join(workerThreads[i], NULL);
        circlePoints += workspaces[i].circlePoints;
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
 *        argv[1] - [Optional] number of points to iterate through
 *        argv[2] - [Optional] number of worker threads to create
 *        argv[3] - [Optional] radius of the circle to calculate
 *
 * @return int of how program exits
 */
int main(int argc, char *argv[]) {
    // Default Values, used if not arguments provided
    int pointCount = 100000;
    int threadCount = 10;
    double radius = 1.0;

    // Retrieving Arguments
    switch(argc){
        case 4:
            radius = atof(argv[3]);
        case 3:
            threadCount = atoi(argv[2]);
        case 2:
            pointCount = atoi(argv[1]);
        default:
            printf("Number of Points = %d, Number of Threads = %d, Circle Radius = %f\n",
                   pointCount, threadCount,radius);
    }


    struct timespec startTime, endTime;
    clock_gettime(CLOCK_REALTIME, &startTime);

    double area = calculateCircleArea(pointCount, threadCount, radius);
    printf("The Area of the circle is: %f\n", area);

    clock_gettime(CLOCK_REALTIME, &endTime);
    double elapsedTime = (endTime.tv_sec - startTime.tv_sec) +
            (endTime.tv_nsec - startTime.tv_nsec) / 1000000000.0;
    printf("Elapsed Time: %f seconds\n", elapsedTime);

    return EXIT_SUCCESS;
}

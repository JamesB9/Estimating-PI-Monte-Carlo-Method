/* STAGE1.C
 *
 * Author: James Burling
 * Date: 12/11/2020
 * Last Modified: 12/11/2020
 *
 */
#include <stdio.h>
#include <stdlib.h>

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
 * Function: calculateCircleArea
 * ------------------------
 * Iterates through a large number of random coordinates, counting how many are within the
 * bounds of a circle. It then calculates a rough estimate for the area of the circle.
 *
 * @param radius     - Radius of the circle to calculate the area of.
 * @param pointCount - Number of random coordinates to iterate through. The greater the
 *                     pointCount, the more accurate the area calculation will be.
 *
 * @return double of the calculated area of the circle
 */
double calculateCircleArea(double radius, int pointCount){
    int circlePoints = 0;

    for(int i = 0; i < pointCount; i++){
        double x = ((double)rand()*2 / (double)RAND_MAX) - 1; // Random double between -1 and 1
        double y = ((double)rand()*2 / (double)RAND_MAX) - 1; // Random double between -1 and 1

        if(isInCircle(radius, x, y)){
            circlePoints++;
        }
    }

    // Returns the area of the circle calculated by:
    // percentage of points in circle * area of the circle's smallest enclosing square
    return ((double)circlePoints/(double)pointCount)*4*radius*radius;
}
/*
 * Function: main
 * ------------------------
 * Iterates through a large number of random coordinates, counting how many are within the
 * bounds of a circle. It then calculates a rough estimate for the area of the circle.
 *
 * @param argc - Number of command line arguments provided
 * @param *argv[] - array of pointers to the command line arguments
 *        argv[0] - The name of the this executable file.
 *        argv[1] - [Optional] number of points to iterate through
 *        argv[2] - [Optional] radius of the circle to calculate
 *
 * @return int of how program exits
 */
int main(int argc, char *argv[]) {
    // Default Values, used if not arguments provided
    int pointCount = 100000;
    double radius = 1.0;

    // Retrieving Arguments
    switch(argc){
        case 3:
            radius = atof(argv[2]);
        case 2:
            pointCount = atoi(argv[1]);
        default:
            printf("Number of Points = %d, Circle Radius = %f\n", pointCount, radius);
    }

    printf("The Area of the circle is: %f\n", calculateCircleArea(radius, pointCount));
    return EXIT_SUCCESS;
}

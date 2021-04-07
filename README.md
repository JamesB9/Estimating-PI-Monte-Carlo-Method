# Estimating PI: Monte Carlo Method

Estimating PI using the Monte Carlo Method was a week-long university assignment. All code is written in C with the assignment focus being on multi-threaded programming.

## How it Works
To estimate PI, the program randomly picks points within a 2x2 square and calculates, using pythagorous' thereom, the distance between the point and the center of the square. If the distance is less than 1, a 'withinCircle' counter is incremented by 1. To calculate PI, 'withinCircle' is divided by the total number of points.

## Project Files

- stage1.c - Single threaded version
- stage2.c - Multi-threaded version with seperate 'withinCircle' counters
- stage3.c - Multi-threaded version where each thread shares the same workspace
 

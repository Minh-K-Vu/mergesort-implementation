/**
 * This file implements parallel mergesort.
 */

#include <stdio.h>
#include <string.h> /* for memcpy */
#include <stdlib.h> /* for malloc */
#include "mergesort.h"

/* this function will be called by mergesort() and also by parallel_mergesort(). */
void merge(int leftstart, int leftend, int rightstart, int rightend){
}

/* this function will be called by parallel_mergesort() as its base case. */
void my_mergesort(int left, int right){
}

/* this function will be called by the testing program. */
void * parallel_mergesort(void *arg){
		// Grab address
		struct argument *args = arg;
		// Check if an argument was provided
		if (args == NULL){
			fprintf(stderr, "parallel_mergesort did not receive an argument\n");
			exit(EXIT_FAILURE);
		}
		// Grab the arguments values
		int left = args->left;
		int right = args->right;
		int level = args->level;
		free(args);
		// Conditions to stop splitting
		// Condition 1: There are only one or less values left;
		if (left >= right) {
			return NULL;
		}
		// Condition 2: Cutoff level has been reached;
		if (level >= cutoff) {
			my_mergesort(left, right);
			return NULL;
		}
		// Calculate the mid point to split range
		int mid = left + (right - left) / 2;
		// Create left side argument
		struct argument *leftArgs = buildArgs(left, mid, level + 1);
		// Create right side argument
		struct argument *rightArgs = buildArgs(mid + 1, right, level + 1);
		// Check if both arguments were sucessfully built
		if (leftArgs == NULL || rightArgs == NULL) {
    		free(leftArgs);
    		free(rightArgs);
    		my_mergesort(left, right);
    		return NULL;
		}
		// Create new threads
		pthread_t leftThread;
		pthread_t rightThread;	
		int leftResult = pthread_create(&leftThread, NULL, parallel_mergesort, leftArgs);
		int rightResult = pthread_create(&rightThread, NULL, parallel_mergesort, rightArgs);
		// Error checking
		if (leftResult != 0) {
			free(leftArgs);
			my_mergesort(left, mid);
		} else {
			int success = pthread_join(leftThread, NULL);
			// If left join fails
			if (success != 0) {
				fprintf(stderr, "pthread_join failed: %s\n", strerror(success));
				exit(EXIT_FAILURE);
			}
		}
		if (rightResult != 0) {
			free(rightArgs);
			my_mergesort(mid + 1, right);
		} else {
			int success = pthread_join(rightThread, NULL);
			// If right join fails
			if (success != 0){
				fprintf(stderr, "pthread_join failed: %s\n", strerror(success));
				exit(EXIT_FAILURE);
			}
		}
		merge(left, mid, mid + 1, right);
		return NULL;
}

/* we build the argument for the parallel_mergesort function. */
struct argument * buildArgs(int left, int right, int level){
		struct argument *args = malloc(sizeof *args);
		// If malloc fails
		if (args == NULL) {
			return NULL;
		}
		// Assign values
		args->left = left;
		args->right = right;
		args->level = level;
		return args;
}


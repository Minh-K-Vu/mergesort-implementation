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
			return NULL;
		}
		// Grab the arguments values
		int left = args->left;
		int right = args->right;
		int level = args->level;
		free(args);
		// Conditions to stop splitting
		// Codition 1: 
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


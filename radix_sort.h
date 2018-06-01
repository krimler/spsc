#pragma once

/* sorts array in ascending order of size "size". */
void radix_sort(int *array, int size);

/* reverses an array of size "size". */
void reverse(int *array, int size);

/* moves all non-zero elements of array to head of array, and fills rest of the element wit zero, at the end. */
void move_array (int *array, int size);

/*search in array arr[] starting from 'l' to last element 'r' for value 'x'. */
int binarySearch(int arr[], int l, int r, int x);

/*------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
/*-----------------------------------------------------*/
//#define TEST_RADIX
#ifdef TEST_RADIX
#define LOGJ printf
//#define LOGD(args...)
#else
#define LOGJ(x...)
#include "log.h"
#include "radix_sort.h"
#endif
/*-----------------------------------------------------*/
static void printArray(int *array, int size)
{

    return;
    int i;
    LOGJ("[ ");
    for (i = 0; i < size; i++)
        LOGJ("%d ", array[i]);
    LOGJ("]\n");
}
/*-----------------------------------------------------*/
static int findLargestNum(int *array, int size)
{

    int i;
    int largestNum = -1;
    for (i = 0; i < size; i++) {
        if (array[i] > largestNum)
            largestNum = array[i];
    }
    return largestNum;
}
/*-----------------------------------------------------*/
// Radix Sort
/* see https://austingwalters.com/radix-sort-in-c/. */
void radix_sort(int *array, int size)
{
    LOGJ("\n\nRunning Radix Sort on Unsorted List!\n\n");
    // Base 10 is used
    int i;
    int semiSorted[size];
    int significantDigit = 1;
    int largestNum = findLargestNum(array, size);
    // Loop until we reach the largest significant digit
    while (largestNum / significantDigit > 0) {
        LOGJ("\tSorting: %d's place ", significantDigit);
        printArray(array, size);
        int bucket[10] = { 0 };
        // Counts the number of "keys" or digits that will go into each bucket
        for (i = 0; i < size; i++)
            bucket[(array[i] / significantDigit) % 10]++;
        /**
         * Add the count of the previous buckets,
         * Acquires the indexes after the end of each bucket location in the array
        	 * Works similar to the count sort algorithm
         **/
        for (i = 1; i < 10; i++)
            bucket[i] += bucket[i - 1];
        // Use the bucket to fill a "semiSorted" array
        for (i = size - 1; i >= 0; i--)
            semiSorted[--bucket[(array[i] / significantDigit) % 10]] = array[i];

        for (i = 0; i < size; i++)
            array[i] = semiSorted[i];

        // Move to next significant digit
        significantDigit *= 10;

        LOGJ("\n\tBucket: ");
        printArray(bucket, 10);
    }
}
/*-----------------------------------------------------*/
void reverse(int *array, int size)
{
    int i;
    int t;
    for (i = 0; i < size / 2; i++) {
        t = array[i];
        array[i] = array[size - 1 - i];
        array[size - 1 - i] = t;
    }
}
/*-----------------------------------------------------*/
void move_array(int *array, int size)
{
    int i;
    for (i = 0; i < size; i++) {
        if (array[i] != 0)
            break;
    }
    if (i == 0) { // nothing to be done.
        return;
    }

    int j = 0;
    for (j = 0; i < size; j++, i++) {
        array[j] = array[i];
    }
    while (j < size) {
        array[j++] = 0;
    }

}
/*-----------------------------------------------------*/
int binarySearch(int arr[], int l, int r, int x)
{
    int i;
    for (i = 0; i < r; i++) {
        if (arr[i] == x)
            return i;
    }
#ifdef BIN_SEARCH
    if (r >= l) {
        int mid = l + (r - l) / 2;

        // If the element is present at the middle
        // itself
        if (arr[mid] == x)
            return mid;

        // If element is smaller than mid, then
        // it can only be present in left subarray
        if (arr[mid] > x)
            return binarySearch(arr, l, mid - 1, x);

        // Else the element can only be present
        // in right subarray
        return binarySearch(arr, mid + 1, r, x);
    }
#endif
    // We reach here when element is not
    // present in array
    return -1;
}
/*-----------------------------------------------------*/
static int test_helper(int *list, int size)
{
    LOGJ("\nUnsorted List: ");
    printArray(list, size);

    radix_sort(list, size);

    LOGJ("\nSorted List:");
    printArray(list, size);
    LOGJ("\n");

    reverse(list, size);
    LOGJ("\n Reverse List:");
    printArray(list, size);
    LOGJ("\n");
    return 0;
}
/*-----------------------------------------------------*/
static int test_even()
{

    LOGJ("\n\nRunning Radix Sort Example in C!\n");
    LOGJ("----------------------------------\n");

    int size = 12;
    int *list = calloc(12, sizeof(int));
    list[0] = 10;
    list[1] = 2;
    list[2] = 303;
    list[3] = 4021;
    list[4] = 293;
    list[5] = 1;
    list[6] =  0;
    list[7] = 429;
    list[8] = 480;
    list[9] = 92;
    list[10] = 2999;
    list[11] = 14;

    return test_helper(list, size);
}
/*-----------------------------------------------------*/
static int test_odd()
{

    LOGJ("\n\nRunning Radix Sort Example in C!\n");
    LOGJ("----------------------------------\n");

    int size = 11;
    int *list = calloc(11, sizeof(int));
    list[0] = 10;
    list[1] = 2;
    list[2] = 303;
    list[3] = 4021;
    list[4] = 293;
    list[5] = 1;
    list[6] =  0;
    list[7] = 429;
    list[8] = 480;
    list[9] = 92;
    list[10] = 2999;

    return test_helper(list, size);
}
/*-----------------------------------------------------*/
static int test_sequence()
{
    int i[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9,};
    test_helper(i, sizeof(i) / sizeof(int) );

    int j[] = {0, 0, 0, 1, 2, 3,};
    test_helper(j, sizeof(j) / sizeof(int) );

    int k[] = {0, 1, 2, 3, 0,};
    return test_helper(k, sizeof(k) / sizeof(int) );
}
/*-----------------------------------------------------*/
static int test_move()
{
    int i[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9,};
    int size = sizeof(i) / sizeof(int);
    LOGJ("\nUnsorted List: ");
    printArray(i, size);

    move_array(i, size);
    LOGJ("\nMoved  List:");
    printArray(i, size);
    LOGJ("\n");
}

static int test_binarySearch()
{
//    int i [] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9,};
    return 0;

}
/*-----------------------------------------------------*/
#ifdef TEST_RADIX
int main(void)
{
    test_odd();
    test_even();
    test_sequence();
    test_move();
}
#endif //TEST_RADIX
/*-----------------------------------------------------*/

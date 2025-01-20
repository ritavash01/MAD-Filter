#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Function for finding the first threshold using MAD filtering:

typedef struct {
    double *data; //pointer to the data array 
    int size;  //size of data array 
    double beta; //Aggressiveness parameter 
    double *filtered_data; // Filtered data 

    //methods 
    double (*calculate_median)(double *, int);
    double (*calculate_MAD)(struct MADFilter *);
    void (*apply_filter)(struct MADFilter *);
} MADFilter;

//***Using qsort for calculation of median*****//

int compare(const void *a, const void *b){  //Helper function for qsort
    double diff = (*(double *)a - *(double *)b);
    return (diff > 0) - (diff < 0); // Return 1, -1, or 0
}

double calculate_median(double *array, int size){
    qsort(array, size, sizeof(double),compare);
    if (size % 2 == 0) {
        return (array[size / 2 - 1] + array[size / 2]) / 2.0;
    } else {
        return array[size / 2];
    }
}
/***Calculation of the MAD filter***/

double calculate_MAD(MADFilter *self){
    double median = self->calculate_median(self->data, self->size);

    //Allocating memory for the absolute deviation of each point from the global median

    double *abs_deviation = (double *)malloc(self->size * sizeof(double));
    if (!abs_deviation) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < self->size; i++) {
        abs_deviation[i] = fabs(self->data[i] - median);
    }
    // Calculate MAD
    double MAD = self->calculate_median(abs_deviation, self->size) * 1.4826;

    free(abs_deviation);
    return MAD;

}

// Method: Apply MAD filtering
void apply_filter(MADFilter *self) {
    double median = self->calculate_median(self->data, self->size);
    double mad = self->calculate_MAD(self);
    double threshold = median + self->beta * mad;

    // Apply the filter
    for (int i = 0; i < self->size; i++) {
        self->filtered_data[i] = (self->data[i] > threshold) ? threshold : self->data[i];
    }
}

// Main function for testing
int main() {
    // Example data
    double data[] = {10.5, 11.0, 10.0, 12.5, 13.0, 14.0, 9.5, 10.8};
    int size = sizeof(data) / sizeof(data[0]);
    double beta = 1.0; // Aggressiveness parameter

    // Create and initialize the MADFilter object
    MADFilter filter;
    filter.data = data;
    filter.size = size;
    filter.beta = beta;
    filter.filtered_data = (double *)malloc(size * sizeof(double));
    if (!filter.filtered_data) {
        perror("Memory allocation failed");
        return EXIT_FAILURE;
    }

    // Assign methods
    filter.calculate_median = calculate_median;
    filter.calculate_MAD = calculate_MAD;
    filter.apply_filter = apply_filter;

    // Apply the MAD filter
    filter.apply_filter(&filter);

    // Print the filtered data
    printf("Filtered data:\n");
    for (int i = 0; i < size; i++) {
        printf("%.2f ", filter.filtered_data[i]);
    }
    printf("\n");

    // Free allocated memory
    free(filter.filtered_data);

    return 0;
}

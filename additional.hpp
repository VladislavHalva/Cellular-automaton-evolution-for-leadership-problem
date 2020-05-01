void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

float maxInArray(float *array, int size){
    float max = 0.0;
    for (int i = 0; i < size; i++){
        if (array[i] > max)
            max = array[i];
    }
    return max;
}
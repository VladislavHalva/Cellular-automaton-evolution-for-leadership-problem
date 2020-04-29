void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int maxInArray(int *array, int size){
    int max = 0;
    for (int i = 0; i < size; i++){
        if (array[i] > max)
            max = array[i];
    }
    return max;
}
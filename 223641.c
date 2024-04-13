void MAFilter(float *dest, const float *source, int start, int end, int N)
{
    for (int i = start; i < end; i++)
    {
        int i1 = i - N / 2;
        int i2 = i + N / 2 + 1;
        if (i1 < start) i1 = start;
        if (i2 > end)   i2 = end;

        double sum = 0;
        for (int j = i1; j < i2; j ++)
        { 
            sum += source[j];
        }
        dest[i] = (float)(sum / (i2 - i1));
    }
}
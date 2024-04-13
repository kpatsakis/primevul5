void hamming(float *w, int N)
{
    for (int i = 0; i < N; i++)
    {
        w[i] = (float)(0.54 - 0.46 * cos(TWOPI * i / (N - 1)));
    }

}
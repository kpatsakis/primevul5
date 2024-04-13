    static void _SaveDebugData(const char *name, const float *data, int minpos, int maxpos, double coeff)
    {
        FILE *fptr = fopen(name, "wt");
        int i;

        if (fptr)
        {
            printf("\nWriting BPM debug data into file %s\n", name);
            for (i = minpos; i < maxpos; i ++)
            {
                fprintf(fptr, "%d\t%.1lf\t%f\n", i, coeff / (double)i, data[i]);
            }
            fclose(fptr);
        }
    }
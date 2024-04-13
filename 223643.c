    void _SaveDebugBeatPos(const char *name, const std::vector<BEAT> &beats)
    {
        printf("\nWriting beat detections data into file %s\n", name);

        FILE *fptr = fopen(name, "wt");
        if (fptr)
        {
            for (uint i = 0; i < beats.size(); i++)
            {
                BEAT b = beats[i];
                fprintf(fptr, "%lf\t%lf\n", b.pos, b.strength);
            }
            fclose(fptr);
        }
    }
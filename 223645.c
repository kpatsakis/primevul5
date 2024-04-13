int BPMDetect::decimate(SAMPLETYPE *dest, const SAMPLETYPE *src, int numsamples)
{
    int count, outcount;
    LONG_SAMPLETYPE out;

    assert(channels > 0);
    assert(decimateBy > 0);
    outcount = 0;
    for (count = 0; count < numsamples; count ++) 
    {
        int j;

        // convert to mono and accumulate
        for (j = 0; j < channels; j ++)
        {
            decimateSum += src[j];
        }
        src += j;

        decimateCount ++;
        if (decimateCount >= decimateBy) 
        {
            // Store every Nth sample only
            out = (LONG_SAMPLETYPE)(decimateSum / (decimateBy * channels));
            decimateSum = 0;
            decimateCount = 0;
#ifdef SOUNDTOUCH_INTEGER_SAMPLES
            // check ranges for sure (shouldn't actually be necessary)
            if (out > 32767) 
            {
                out = 32767;
            } 
            else if (out < -32768) 
            {
                out = -32768;
            }
#endif // SOUNDTOUCH_INTEGER_SAMPLES
            dest[outcount] = (SAMPLETYPE)out;
            outcount ++;
        }
    }
    return outcount;
}
void BPMDetect::inputSamples(const SAMPLETYPE *samples, int numSamples)
{
    SAMPLETYPE decimated[DECIMATED_BLOCK_SIZE];

    // iterate so that max INPUT_BLOCK_SAMPLES processed per iteration
    while (numSamples > 0)
    {
        int block;
        int decSamples;

        block = (numSamples > INPUT_BLOCK_SIZE) ? INPUT_BLOCK_SIZE : numSamples;

        // decimate. note that converts to mono at the same time
        decSamples = decimate(decimated, samples, block);
        samples += block * channels;
        numSamples -= block;

        buffer->putSamples(decimated, decSamples);
    }

    // when the buffer has enough samples for processing...
    int req = max(windowLen + XCORR_UPDATE_SEQUENCE, 2 * XCORR_UPDATE_SEQUENCE);
    while ((int)buffer->numSamples() >= req) 
    {
        // ... update autocorrelations...
        updateXCorr(XCORR_UPDATE_SEQUENCE);
        // ...update beat position calculation...
        updateBeatPos(XCORR_UPDATE_SEQUENCE / 2);
        // ... and remove proceessed samples from the buffer
        int n = XCORR_UPDATE_SEQUENCE / OVERLAP_FACTOR;
        buffer->receiveSamples(n);
    }
}
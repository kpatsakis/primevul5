void BPMDetect::updateXCorr(int process_samples)
{
    int offs;
    SAMPLETYPE *pBuffer;
    
    assert(buffer->numSamples() >= (uint)(process_samples + windowLen));
    assert(process_samples == XCORR_UPDATE_SEQUENCE);

    pBuffer = buffer->ptrBegin();

    // calculate decay factor for xcorr filtering
    float xcorr_decay = (float)pow(0.5, 1.0 / (XCORR_DECAY_TIME_CONSTANT * TARGET_SRATE / process_samples));

    // prescale pbuffer
    float tmp[XCORR_UPDATE_SEQUENCE];
    for (int i = 0; i < process_samples; i++)
    {
        tmp[i] = hamw[i] * hamw[i] * pBuffer[i];
    }

    #pragma omp parallel for
    for (offs = windowStart; offs < windowLen; offs ++) 
    {
        double sum;
        int i;

        sum = 0;
        for (i = 0; i < process_samples; i ++) 
        {
            sum += tmp[i] * pBuffer[i + offs];  // scaling the sub-result shouldn't be necessary
        }
        xcorr[offs] *= xcorr_decay;   // decay 'xcorr' here with suitable time constant.

        xcorr[offs] += (float)fabs(sum);
    }
}
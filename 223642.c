void BPMDetect::updateBeatPos(int process_samples)
{
    SAMPLETYPE *pBuffer;

    assert(buffer->numSamples() >= (uint)(process_samples + windowLen));

    pBuffer = buffer->ptrBegin();
    assert(process_samples == XCORR_UPDATE_SEQUENCE / 2);

    //    static double thr = 0.0003;
    double posScale = (double)this->decimateBy / (double)this->sampleRate;
    int resetDur = (int)(0.12 / posScale + 0.5);
    double corrScale = 1.0 / (double)(windowLen - windowStart);

    // prescale pbuffer
    float tmp[XCORR_UPDATE_SEQUENCE / 2];
    for (int i = 0; i < process_samples; i++)
    {
        tmp[i] = hamw2[i] * hamw2[i] * pBuffer[i];
    }

    #pragma omp parallel for
    for (int offs = windowStart; offs < windowLen; offs++)
    {
        double sum = 0;
        for (int i = 0; i < process_samples; i++)
        {
            sum += tmp[i] * pBuffer[offs + i];
        }
        beatcorr_ringbuff[(beatcorr_ringbuffpos + offs) % windowLen] += (float)((sum > 0) ? sum : 0); // accumulate only positive correlations
    }

    int skipstep = XCORR_UPDATE_SEQUENCE / OVERLAP_FACTOR;

    // compensate empty buffer at beginning by scaling coefficient
    float scale = (float)windowLen / (float)(skipstep * init_scaler);
    if (scale > 1.0f)
    {
        init_scaler++;
    }
    else
    {
        scale = 1.0f;
    }

    // detect beats
    for (int i = 0; i < skipstep; i++)
    {
        LONG_SAMPLETYPE max = 0;

        float sum = beatcorr_ringbuff[beatcorr_ringbuffpos];
        sum -= beat_lpf.update(sum);

        if (sum > peakVal)
        {
            // found new local largest value
            peakVal = sum;
            peakPos = pos;
        }
        if (pos > peakPos + resetDur)
        {
            // largest value not updated for 200msec => accept as beat
            peakPos += skipstep;
            if (peakVal > 0)
            {
                // add detected beat to end of "beats" vector
                BEAT temp = { (float)(peakPos * posScale), (float)(peakVal * scale) };
                beats.push_back(temp);
            }

            peakVal = 0;
            peakPos = pos;
        }

        beatcorr_ringbuff[beatcorr_ringbuffpos] = 0;
        pos++;
        beatcorr_ringbuffpos = (beatcorr_ringbuffpos + 1) % windowLen;
    }
}
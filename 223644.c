float BPMDetect::getBpm()
{
    double peakPos;
    double coeff;
    PeakFinder peakFinder;

    // remove bias from xcorr data
    removeBias();

    coeff = 60.0 * ((double)sampleRate / (double)decimateBy);

    // save bpm debug data if debug data writing enabled
    _SaveDebugData("soundtouch-bpm-xcorr.txt", xcorr, windowStart, windowLen, coeff);

    // Smoothen by N-point moving-average
    float *data = new float[windowLen];
    memset(data, 0, sizeof(float) * windowLen);
    MAFilter(data, xcorr, windowStart, windowLen, MOVING_AVERAGE_N);

    // find peak position
    peakPos = peakFinder.detectPeak(data, windowStart, windowLen);

    // save bpm debug data if debug data writing enabled
    _SaveDebugData("soundtouch-bpm-smoothed.txt", data, windowStart, windowLen, coeff);

    delete[] data;

    assert(decimateBy != 0);
    if (peakPos < 1e-9) return 0.0; // detection failed.

    _SaveDebugBeatPos("soundtouch-detected-beats.txt", beats);

    // calculate BPM
    float bpm = (float)(coeff / peakPos);
    return (bpm >= MIN_BPM && bpm <= MAX_BPM_VALID) ? bpm : 0;
}
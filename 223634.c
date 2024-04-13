BPMDetect::~BPMDetect()
{
    delete[] xcorr;
    delete[] beatcorr_ringbuff;
    delete[] hamw;
    delete[] hamw2;
    delete buffer;
}
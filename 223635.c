void BPMDetect::removeBias()
{
    int i;

    // Remove linear bias: calculate linear regression coefficient
    // 1. calc mean of 'xcorr' and 'i'
    double mean_i = 0;
    double mean_x = 0;
    for (i = windowStart; i < windowLen; i++)
    {
        mean_x += xcorr[i];
    }
    mean_x /= (windowLen - windowStart);
    mean_i = 0.5 * (windowLen - 1 + windowStart);

    // 2. calculate linear regression coefficient
    double b = 0;
    double div = 0;
    for (i = windowStart; i < windowLen; i++)
    {
        double xt = xcorr[i] - mean_x;
        double xi = i - mean_i;
        b += xt * xi;
        div += xi * xi;
    }
    b /= div;

    // subtract linear regression and resolve min. value bias
    float minval = FLT_MAX;   // arbitrary large number
    for (i = windowStart; i < windowLen; i ++)
    {
        xcorr[i] -= (float)(b * i);
        if (xcorr[i] < minval)
        {
            minval = xcorr[i];
        }
    }

    // subtract min.value
    for (i = windowStart; i < windowLen; i ++)
    {
        xcorr[i] -= minval;
    }
}
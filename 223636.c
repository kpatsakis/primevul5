int BPMDetect::getBeats(float *pos, float *values, int max_num)
{
    int num = beats.size();
    if ((!pos) || (!values)) return num;    // pos or values NULL, return just size

    for (int i = 0; (i < num) && (i < max_num); i++)
    {
        pos[i] = beats[i].pos;
        values[i] = beats[i].strength;
    }
    return num;
}
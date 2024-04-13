float IIR2_filter::update(float x)
{
    prev[0] = x;
    double y = x * coeffs[0];

    for (int i = 4; i >= 1; i--)
    {
        y += coeffs[i] * prev[i];
        prev[i] = prev[i - 1];
    }

    prev[3] = y;
    return (float)y;
}
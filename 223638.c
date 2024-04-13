IIR2_filter::IIR2_filter(const double *lpf_coeffs)
{
    memcpy(coeffs, lpf_coeffs, 5 * sizeof(double));
    memset(prev, 0, sizeof(prev));
}
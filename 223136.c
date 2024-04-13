OPJ_BOOL opj_tcd_is_band_empty(opj_tcd_band_t* band)
{
    return (band->x1 - band->x0 == 0) || (band->y1 - band->y0 == 0);
}
void opj_tcd_rateallocate_fixed(opj_tcd_t *tcd)
{
    OPJ_UINT32 layno;

    for (layno = 0; layno < tcd->tcp->numlayers; layno++) {
        opj_tcd_makelayer_fixed(tcd, layno, 1);
    }
}
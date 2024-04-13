static OPJ_BOOL opj_tcd_is_whole_tilecomp_decoding(opj_tcd_t *p_tcd,
        OPJ_UINT32 compno)
{
    opj_tcd_tilecomp_t* tilec = &(p_tcd->tcd_image->tiles->comps[compno]);
    opj_image_comp_t* image_comp = &(p_tcd->image->comps[compno]);
    /* Compute the intersection of the area of interest, expressed in tile coordinates */
    /* with the tile coordinates */
    OPJ_UINT32 tcx0 = opj_uint_max(
                          (OPJ_UINT32)tilec->x0,
                          opj_uint_ceildiv(p_tcd->win_x0, image_comp->dx));
    OPJ_UINT32 tcy0 = opj_uint_max(
                          (OPJ_UINT32)tilec->y0,
                          opj_uint_ceildiv(p_tcd->win_y0, image_comp->dy));
    OPJ_UINT32 tcx1 = opj_uint_min(
                          (OPJ_UINT32)tilec->x1,
                          opj_uint_ceildiv(p_tcd->win_x1, image_comp->dx));
    OPJ_UINT32 tcy1 = opj_uint_min(
                          (OPJ_UINT32)tilec->y1,
                          opj_uint_ceildiv(p_tcd->win_y1, image_comp->dy));

    OPJ_UINT32 shift = tilec->numresolutions - tilec->minimum_num_resolutions;
    /* Tolerate small margin within the reduced resolution factor to consider if */
    /* the whole tile path must be taken */
    return (tcx0 >= (OPJ_UINT32)tilec->x0 &&
            tcy0 >= (OPJ_UINT32)tilec->y0 &&
            tcx1 <= (OPJ_UINT32)tilec->x1 &&
            tcy1 <= (OPJ_UINT32)tilec->y1 &&
            (shift >= 32 ||
             (((tcx0 - (OPJ_UINT32)tilec->x0) >> shift) == 0 &&
              ((tcy0 - (OPJ_UINT32)tilec->y0) >> shift) == 0 &&
              (((OPJ_UINT32)tilec->x1 - tcx1) >> shift) == 0 &&
              (((OPJ_UINT32)tilec->y1 - tcy1) >> shift) == 0)));
}
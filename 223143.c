static void opj_tcd_free_tile(opj_tcd_t *p_tcd)
{
    OPJ_UINT32 compno, resno, bandno, precno;
    opj_tcd_tile_t *l_tile = 00;
    opj_tcd_tilecomp_t *l_tile_comp = 00;
    opj_tcd_resolution_t *l_res = 00;
    opj_tcd_band_t *l_band = 00;
    opj_tcd_precinct_t *l_precinct = 00;
    OPJ_UINT32 l_nb_resolutions, l_nb_precincts;
    void (* l_tcd_code_block_deallocate)(opj_tcd_precinct_t *) = 00;

    if (! p_tcd) {
        return;
    }

    if (! p_tcd->tcd_image) {
        return;
    }

    if (p_tcd->m_is_decoder) {
        l_tcd_code_block_deallocate = opj_tcd_code_block_dec_deallocate;
    } else {
        l_tcd_code_block_deallocate = opj_tcd_code_block_enc_deallocate;
    }

    l_tile = p_tcd->tcd_image->tiles;
    if (! l_tile) {
        return;
    }

    l_tile_comp = l_tile->comps;

    for (compno = 0; compno < l_tile->numcomps; ++compno) {
        l_res = l_tile_comp->resolutions;
        if (l_res) {

            l_nb_resolutions = l_tile_comp->resolutions_size / (OPJ_UINT32)sizeof(
                                   opj_tcd_resolution_t);
            for (resno = 0; resno < l_nb_resolutions; ++resno) {
                l_band = l_res->bands;
                for (bandno = 0; bandno < 3; ++bandno) {
                    l_precinct = l_band->precincts;
                    if (l_precinct) {

                        l_nb_precincts = l_band->precincts_data_size / (OPJ_UINT32)sizeof(
                                             opj_tcd_precinct_t);
                        for (precno = 0; precno < l_nb_precincts; ++precno) {
                            opj_tgt_destroy(l_precinct->incltree);
                            l_precinct->incltree = 00;
                            opj_tgt_destroy(l_precinct->imsbtree);
                            l_precinct->imsbtree = 00;
                            (*l_tcd_code_block_deallocate)(l_precinct);
                            ++l_precinct;
                        }

                        opj_free(l_band->precincts);
                        l_band->precincts = 00;
                    }
                    ++l_band;
                } /* for (resno */
                ++l_res;
            }

            opj_free(l_tile_comp->resolutions);
            l_tile_comp->resolutions = 00;
        }

        if (l_tile_comp->ownsData && l_tile_comp->data) {
            opj_image_data_free(l_tile_comp->data);
            l_tile_comp->data = 00;
            l_tile_comp->ownsData = 0;
            l_tile_comp->data_size = 0;
            l_tile_comp->data_size_needed = 0;
        }

        opj_image_data_free(l_tile_comp->data_win);

        ++l_tile_comp;
    }

    opj_free(l_tile->comps);
    l_tile->comps = 00;
    opj_free(p_tcd->tcd_image->tiles);
    p_tcd->tcd_image->tiles = 00;
}
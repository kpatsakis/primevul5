void opj_tcd_makelayer(opj_tcd_t *tcd,
                       OPJ_UINT32 layno,
                       OPJ_FLOAT64 thresh,
                       OPJ_UINT32 final)
{
    OPJ_UINT32 compno, resno, bandno, precno, cblkno;
    OPJ_UINT32 passno;

    opj_tcd_tile_t *tcd_tile = tcd->tcd_image->tiles;

    tcd_tile->distolayer[layno] = 0;        /* fixed_quality */

    for (compno = 0; compno < tcd_tile->numcomps; compno++) {
        opj_tcd_tilecomp_t *tilec = &tcd_tile->comps[compno];

        for (resno = 0; resno < tilec->numresolutions; resno++) {
            opj_tcd_resolution_t *res = &tilec->resolutions[resno];

            for (bandno = 0; bandno < res->numbands; bandno++) {
                opj_tcd_band_t *band = &res->bands[bandno];

                /* Skip empty bands */
                if (opj_tcd_is_band_empty(band)) {
                    continue;
                }

                for (precno = 0; precno < res->pw * res->ph; precno++) {
                    opj_tcd_precinct_t *prc = &band->precincts[precno];

                    for (cblkno = 0; cblkno < prc->cw * prc->ch; cblkno++) {
                        opj_tcd_cblk_enc_t *cblk = &prc->cblks.enc[cblkno];
                        opj_tcd_layer_t *layer = &cblk->layers[layno];
                        OPJ_UINT32 n;

                        if (layno == 0) {
                            cblk->numpassesinlayers = 0;
                        }

                        n = cblk->numpassesinlayers;

                        if (thresh < 0) {
                            /* Special value to indicate to use all passes */
                            n = cblk->totalpasses;
                        } else {
                            for (passno = cblk->numpassesinlayers; passno < cblk->totalpasses; passno++) {
                                OPJ_UINT32 dr;
                                OPJ_FLOAT64 dd;
                                opj_tcd_pass_t *pass = &cblk->passes[passno];

                                if (n == 0) {
                                    dr = pass->rate;
                                    dd = pass->distortiondec;
                                } else {
                                    dr = pass->rate - cblk->passes[n - 1].rate;
                                    dd = pass->distortiondec - cblk->passes[n - 1].distortiondec;
                                }

                                if (!dr) {
                                    if (dd != 0) {
                                        n = passno + 1;
                                    }
                                    continue;
                                }
                                if (thresh - (dd / dr) <
                                        DBL_EPSILON) { /* do not rely on float equality, check with DBL_EPSILON margin */
                                    n = passno + 1;
                                }
                            }
                        }

                        layer->numpasses = n - cblk->numpassesinlayers;

                        if (!layer->numpasses) {
                            layer->disto = 0;
                            continue;
                        }

                        if (cblk->numpassesinlayers == 0) {
                            layer->len = cblk->passes[n - 1].rate;
                            layer->data = cblk->data;
                            layer->disto = cblk->passes[n - 1].distortiondec;
                        } else {
                            layer->len = cblk->passes[n - 1].rate - cblk->passes[cblk->numpassesinlayers -
                                         1].rate;
                            layer->data = cblk->data + cblk->passes[cblk->numpassesinlayers - 1].rate;
                            layer->disto = cblk->passes[n - 1].distortiondec -
                                           cblk->passes[cblk->numpassesinlayers - 1].distortiondec;
                        }

                        tcd_tile->distolayer[layno] += layer->disto;    /* fixed_quality */

                        if (final) {
                            cblk->numpassesinlayers = n;
                        }
                    }
                }
            }
        }
    }
}
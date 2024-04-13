void opj_tcd_makelayer_fixed(opj_tcd_t *tcd, OPJ_UINT32 layno,
                             OPJ_UINT32 final)
{
    OPJ_UINT32 compno, resno, bandno, precno, cblkno;
    OPJ_INT32 value;                        /*, matrice[tcd_tcp->numlayers][tcd_tile->comps[0].numresolutions][3]; */
    OPJ_INT32 matrice[10][10][3];
    OPJ_UINT32 i, j, k;

    opj_cp_t *cp = tcd->cp;
    opj_tcd_tile_t *tcd_tile = tcd->tcd_image->tiles;
    opj_tcp_t *tcd_tcp = tcd->tcp;

    for (compno = 0; compno < tcd_tile->numcomps; compno++) {
        opj_tcd_tilecomp_t *tilec = &tcd_tile->comps[compno];

        for (i = 0; i < tcd_tcp->numlayers; i++) {
            for (j = 0; j < tilec->numresolutions; j++) {
                for (k = 0; k < 3; k++) {
                    matrice[i][j][k] =
                        (OPJ_INT32)((OPJ_FLOAT32)cp->m_specific_param.m_enc.m_matrice[i *
                                      tilec->numresolutions * 3 + j * 3 + k]
                                    * (OPJ_FLOAT32)(tcd->image->comps[compno].prec / 16.0));
                }
            }
        }

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
                        OPJ_INT32 imsb = (OPJ_INT32)(tcd->image->comps[compno].prec -
                                                     cblk->numbps); /* number of bit-plan equal to zero */

                        /* Correction of the matrix of coefficient to include the IMSB information */
                        if (layno == 0) {
                            value = matrice[layno][resno][bandno];
                            if (imsb >= value) {
                                value = 0;
                            } else {
                                value -= imsb;
                            }
                        } else {
                            value = matrice[layno][resno][bandno] - matrice[layno - 1][resno][bandno];
                            if (imsb >= matrice[layno - 1][resno][bandno]) {
                                value -= (imsb - matrice[layno - 1][resno][bandno]);
                                if (value < 0) {
                                    value = 0;
                                }
                            }
                        }

                        if (layno == 0) {
                            cblk->numpassesinlayers = 0;
                        }

                        n = cblk->numpassesinlayers;
                        if (cblk->numpassesinlayers == 0) {
                            if (value != 0) {
                                n = 3 * (OPJ_UINT32)value - 2 + cblk->numpassesinlayers;
                            } else {
                                n = cblk->numpassesinlayers;
                            }
                        } else {
                            n = 3 * (OPJ_UINT32)value + cblk->numpassesinlayers;
                        }

                        layer->numpasses = n - cblk->numpassesinlayers;

                        if (!layer->numpasses) {
                            continue;
                        }

                        if (cblk->numpassesinlayers == 0) {
                            layer->len = cblk->passes[n - 1].rate;
                            layer->data = cblk->data;
                        } else {
                            layer->len = cblk->passes[n - 1].rate - cblk->passes[cblk->numpassesinlayers -
                                         1].rate;
                            layer->data = cblk->data + cblk->passes[cblk->numpassesinlayers - 1].rate;
                        }

                        if (final) {
                            cblk->numpassesinlayers = n;
                        }
                    }
                }
            }
        }
    }
}
void tcd_dump(FILE *fd, opj_tcd_t *tcd, opj_tcd_image_t * img)
{
    int tileno, compno, resno, bandno, precno;/*, cblkno;*/

    fprintf(fd, "image {\n");
    fprintf(fd, "  tw=%d, th=%d x0=%d x1=%d y0=%d y1=%d\n",
            img->tw, img->th, tcd->image->x0, tcd->image->x1, tcd->image->y0,
            tcd->image->y1);

    for (tileno = 0; tileno < img->th * img->tw; tileno++) {
        opj_tcd_tile_t *tile = &tcd->tcd_image->tiles[tileno];
        fprintf(fd, "  tile {\n");
        fprintf(fd, "    x0=%d, y0=%d, x1=%d, y1=%d, numcomps=%d\n",
                tile->x0, tile->y0, tile->x1, tile->y1, tile->numcomps);
        for (compno = 0; compno < tile->numcomps; compno++) {
            opj_tcd_tilecomp_t *tilec = &tile->comps[compno];
            fprintf(fd, "    tilec {\n");
            fprintf(fd,
                    "      x0=%d, y0=%d, x1=%d, y1=%d, numresolutions=%d\n",
                    tilec->x0, tilec->y0, tilec->x1, tilec->y1, tilec->numresolutions);
            for (resno = 0; resno < tilec->numresolutions; resno++) {
                opj_tcd_resolution_t *res = &tilec->resolutions[resno];
                fprintf(fd, "\n   res {\n");
                fprintf(fd,
                        "          x0=%d, y0=%d, x1=%d, y1=%d, pw=%d, ph=%d, numbands=%d\n",
                        res->x0, res->y0, res->x1, res->y1, res->pw, res->ph, res->numbands);
                for (bandno = 0; bandno < res->numbands; bandno++) {
                    opj_tcd_band_t *band = &res->bands[bandno];
                    fprintf(fd, "        band {\n");
                    fprintf(fd,
                            "          x0=%d, y0=%d, x1=%d, y1=%d, stepsize=%f, numbps=%d\n",
                            band->x0, band->y0, band->x1, band->y1, band->stepsize, band->numbps);
                    for (precno = 0; precno < res->pw * res->ph; precno++) {
                        opj_tcd_precinct_t *prec = &band->precincts[precno];
                        fprintf(fd, "          prec {\n");
                        fprintf(fd,
                                "            x0=%d, y0=%d, x1=%d, y1=%d, cw=%d, ch=%d\n",
                                prec->x0, prec->y0, prec->x1, prec->y1, prec->cw, prec->ch);
                        /*
                        for (cblkno = 0; cblkno < prec->cw * prec->ch; cblkno++) {
                                opj_tcd_cblk_t *cblk = &prec->cblks[cblkno];
                                fprintf(fd, "            cblk {\n");
                                fprintf(fd,
                                        "              x0=%d, y0=%d, x1=%d, y1=%d\n",
                                        cblk->x0, cblk->y0, cblk->x1, cblk->y1);
                                fprintf(fd, "            }\n");
                        }
                        */
                        fprintf(fd, "          }\n");
                    }
                    fprintf(fd, "        }\n");
                }
                fprintf(fd, "      }\n");
            }
            fprintf(fd, "    }\n");
        }
        fprintf(fd, "  }\n");
    }
    fprintf(fd, "}\n");
}
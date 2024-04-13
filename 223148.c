void opj_tcd_destroy(opj_tcd_t *tcd)
{
    if (tcd) {
        opj_tcd_free_tile(tcd);

        if (tcd->tcd_image) {
            opj_free(tcd->tcd_image);
            tcd->tcd_image = 00;
        }

        opj_free(tcd->used_component);

        opj_free(tcd);
    }
}
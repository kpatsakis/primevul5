OPJ_BOOL opj_tcd_init_encode_tile(opj_tcd_t *p_tcd, OPJ_UINT32 p_tile_no,
                                  opj_event_mgr_t* p_manager)
{
    return opj_tcd_init_tile(p_tcd, p_tile_no, OPJ_TRUE,
                             sizeof(opj_tcd_cblk_enc_t), p_manager);
}
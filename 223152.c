static OPJ_BOOL opj_tcd_code_block_enc_allocate(opj_tcd_cblk_enc_t *
        p_code_block)
{
    if (! p_code_block->layers) {
        /* no memset since data */
        p_code_block->layers = (opj_tcd_layer_t*) opj_calloc(100,
                               sizeof(opj_tcd_layer_t));
        if (! p_code_block->layers) {
            return OPJ_FALSE;
        }
    }
    if (! p_code_block->passes) {
        p_code_block->passes = (opj_tcd_pass_t*) opj_calloc(100,
                               sizeof(opj_tcd_pass_t));
        if (! p_code_block->passes) {
            return OPJ_FALSE;
        }
    }
    return OPJ_TRUE;
}
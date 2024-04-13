static OPJ_BOOL opj_j2k_write_epc(opj_j2k_t *p_j2k,
                                  struct opj_stream_private *p_stream,
                                  struct opj_event_mgr * p_manager)
{
    opj_codestream_index_t * l_cstr_index = 00;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);
    assert(p_stream != 00);

    OPJ_UNUSED(p_manager);

    l_cstr_index = p_j2k->cstr_index;
    if (l_cstr_index) {
        l_cstr_index->codestream_size = (OPJ_UINT64)opj_stream_tell(p_stream);
        /* UniPG>> */
        /* The following adjustment is done to adjust the codestream size */
        /* if SOD is not at 0 in the buffer. Useful in case of JP2, where */
        /* the first bunch of bytes is not in the codestream              */
        l_cstr_index->codestream_size -= (OPJ_UINT64)l_cstr_index->main_head_start;
        /* <<UniPG */
    }

#ifdef USE_JPWL
    /* preparation of JPWL marker segments */
#if 0
    if (cp->epc_on) {

        /* encode according to JPWL */
        jpwl_encode(p_j2k, p_stream, image);

    }
#endif
    assert(0 && "TODO");
#endif /* USE_JPWL */

    return OPJ_TRUE;
}
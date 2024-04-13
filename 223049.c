OPJ_BOOL opj_j2k_start_compress(opj_j2k_t *p_j2k,
                                opj_stream_private_t *p_stream,
                                opj_image_t * p_image,
                                opj_event_mgr_t * p_manager)
{
    /* preconditions */
    assert(p_j2k != 00);
    assert(p_stream != 00);
    assert(p_manager != 00);

    p_j2k->m_private_image = opj_image_create0();
    if (! p_j2k->m_private_image) {
        opj_event_msg(p_manager, EVT_ERROR, "Failed to allocate image header.");
        return OPJ_FALSE;
    }
    opj_copy_image_header(p_image, p_j2k->m_private_image);

    /* TODO_MSD: Find a better way */
    if (p_image->comps) {
        OPJ_UINT32 it_comp;
        for (it_comp = 0 ; it_comp < p_image->numcomps; it_comp++) {
            if (p_image->comps[it_comp].data) {
                p_j2k->m_private_image->comps[it_comp].data = p_image->comps[it_comp].data;
                p_image->comps[it_comp].data = NULL;

            }
        }
    }

    /* customization of the validation */
    if (! opj_j2k_setup_encoding_validation(p_j2k, p_manager)) {
        return OPJ_FALSE;
    }

    /* validation of the parameters codec */
    if (! opj_j2k_exec(p_j2k, p_j2k->m_validation_list, p_stream, p_manager)) {
        return OPJ_FALSE;
    }

    /* customization of the encoding */
    if (! opj_j2k_setup_header_writing(p_j2k, p_manager)) {
        return OPJ_FALSE;
    }

    /* write header */
    if (! opj_j2k_exec(p_j2k, p_j2k->m_procedure_list, p_stream, p_manager)) {
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}
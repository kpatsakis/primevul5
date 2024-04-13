OPJ_BOOL opj_j2k_read_header(opj_stream_private_t *p_stream,
                             opj_j2k_t* p_j2k,
                             opj_image_t** p_image,
                             opj_event_mgr_t* p_manager)
{
    /* preconditions */
    assert(p_j2k != 00);
    assert(p_stream != 00);
    assert(p_manager != 00);

    /* create an empty image header */
    p_j2k->m_private_image = opj_image_create0();
    if (! p_j2k->m_private_image) {
        return OPJ_FALSE;
    }

    /* customization of the validation */
    if (! opj_j2k_setup_decoding_validation(p_j2k, p_manager)) {
        opj_image_destroy(p_j2k->m_private_image);
        p_j2k->m_private_image = NULL;
        return OPJ_FALSE;
    }

    /* validation of the parameters codec */
    if (! opj_j2k_exec(p_j2k, p_j2k->m_validation_list, p_stream, p_manager)) {
        opj_image_destroy(p_j2k->m_private_image);
        p_j2k->m_private_image = NULL;
        return OPJ_FALSE;
    }

    /* customization of the encoding */
    if (! opj_j2k_setup_header_reading(p_j2k, p_manager)) {
        opj_image_destroy(p_j2k->m_private_image);
        p_j2k->m_private_image = NULL;
        return OPJ_FALSE;
    }

    /* read header */
    if (! opj_j2k_exec(p_j2k, p_j2k->m_procedure_list, p_stream, p_manager)) {
        opj_image_destroy(p_j2k->m_private_image);
        p_j2k->m_private_image = NULL;
        return OPJ_FALSE;
    }

    *p_image = opj_image_create0();
    if (!(*p_image)) {
        return OPJ_FALSE;
    }

    /* Copy codestream image information to the output image */
    opj_copy_image_header(p_j2k->m_private_image, *p_image);

    /*Allocate and initialize some elements of codestrem index*/
    if (!opj_j2k_allocate_tile_element_cstr_index(p_j2k)) {
        opj_image_destroy(*p_image);
        *p_image = NULL;
        return OPJ_FALSE;
    }

    return OPJ_TRUE;
}
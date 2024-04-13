OPJ_BOOL opj_j2k_decode(opj_j2k_t * p_j2k,
                        opj_stream_private_t * p_stream,
                        opj_image_t * p_image,
                        opj_event_mgr_t * p_manager)
{
    if (!p_image) {
        return OPJ_FALSE;
    }

    /* Heuristics to detect sequence opj_read_header(), opj_set_decoded_resolution_factor() */
    /* and finally opj_decode_image() without manual setting of comps[].factor */
    /* We could potentially always execute it, if we don't allow people to do */
    /* opj_read_header(), modify x0,y0,x1,y1 of returned image an call opj_decode_image() */
    if (p_j2k->m_cp.m_specific_param.m_dec.m_reduce > 0 &&
            p_j2k->m_private_image != NULL &&
            p_j2k->m_private_image->numcomps > 0 &&
            p_j2k->m_private_image->comps[0].factor ==
            p_j2k->m_cp.m_specific_param.m_dec.m_reduce &&
            p_image->numcomps > 0 &&
            p_image->comps[0].factor == 0 &&
            /* Don't mess with image dimension if the user has allocated it */
            p_image->comps[0].data == NULL) {
        OPJ_UINT32 it_comp;

        /* Update the comps[].factor member of the output image with the one */
        /* of m_reduce */
        for (it_comp = 0; it_comp < p_image->numcomps; ++it_comp) {
            p_image->comps[it_comp].factor = p_j2k->m_cp.m_specific_param.m_dec.m_reduce;
        }
        if (!opj_j2k_update_image_dimensions(p_image, p_manager)) {
            return OPJ_FALSE;
        }
    }

    if (p_j2k->m_output_image == NULL) {
        p_j2k->m_output_image = opj_image_create0();
        if (!(p_j2k->m_output_image)) {
            return OPJ_FALSE;
        }
    }
    opj_copy_image_header(p_image, p_j2k->m_output_image);

    /* customization of the decoding */
    if (!opj_j2k_setup_decoding(p_j2k, p_manager)) {
        return OPJ_FALSE;
    }

    /* Decode the codestream */
    if (! opj_j2k_exec(p_j2k, p_j2k->m_procedure_list, p_stream, p_manager)) {
        opj_image_destroy(p_j2k->m_private_image);
        p_j2k->m_private_image = NULL;
        return OPJ_FALSE;
    }

    /* Move data and copy one information from codec to output image*/
    return opj_j2k_move_data_from_codec_to_output_image(p_j2k, p_image);
}
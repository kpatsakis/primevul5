static OPJ_BOOL opj_j2k_move_data_from_codec_to_output_image(opj_j2k_t * p_j2k,
        opj_image_t * p_image)
{
    OPJ_UINT32 compno;

    /* Move data and copy one information from codec to output image*/
    if (p_j2k->m_specific_param.m_decoder.m_numcomps_to_decode > 0) {
        opj_image_comp_t* newcomps =
            (opj_image_comp_t*) opj_malloc(
                p_j2k->m_specific_param.m_decoder.m_numcomps_to_decode *
                sizeof(opj_image_comp_t));
        if (newcomps == NULL) {
            opj_image_destroy(p_j2k->m_private_image);
            p_j2k->m_private_image = NULL;
            return OPJ_FALSE;
        }
        for (compno = 0; compno < p_image->numcomps; compno++) {
            opj_image_data_free(p_image->comps[compno].data);
            p_image->comps[compno].data = NULL;
        }
        for (compno = 0;
                compno < p_j2k->m_specific_param.m_decoder.m_numcomps_to_decode; compno++) {
            OPJ_UINT32 src_compno =
                p_j2k->m_specific_param.m_decoder.m_comps_indices_to_decode[compno];
            memcpy(&(newcomps[compno]),
                   &(p_j2k->m_output_image->comps[src_compno]),
                   sizeof(opj_image_comp_t));
            newcomps[compno].resno_decoded =
                p_j2k->m_output_image->comps[src_compno].resno_decoded;
            newcomps[compno].data = p_j2k->m_output_image->comps[src_compno].data;
            p_j2k->m_output_image->comps[src_compno].data = NULL;
        }
        for (compno = 0; compno < p_image->numcomps; compno++) {
            assert(p_j2k->m_output_image->comps[compno].data == NULL);
            opj_image_data_free(p_j2k->m_output_image->comps[compno].data);
            p_j2k->m_output_image->comps[compno].data = NULL;
        }
        p_image->numcomps = p_j2k->m_specific_param.m_decoder.m_numcomps_to_decode;
        opj_free(p_image->comps);
        p_image->comps = newcomps;
    } else {
        for (compno = 0; compno < p_image->numcomps; compno++) {
            p_image->comps[compno].resno_decoded =
                p_j2k->m_output_image->comps[compno].resno_decoded;
            opj_image_data_free(p_image->comps[compno].data);
            p_image->comps[compno].data = p_j2k->m_output_image->comps[compno].data;
#if 0
            char fn[256];
            sprintf(fn, "/tmp/%d.raw", compno);
            FILE *debug = fopen(fn, "wb");
            fwrite(p_image->comps[compno].data, sizeof(OPJ_INT32),
                   p_image->comps[compno].w * p_image->comps[compno].h, debug);
            fclose(debug);
#endif
            p_j2k->m_output_image->comps[compno].data = NULL;
        }
    }
    return OPJ_TRUE;
}
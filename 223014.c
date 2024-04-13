static opj_codestream_index_t* opj_j2k_create_cstr_index(void)
{
    opj_codestream_index_t* cstr_index = (opj_codestream_index_t*)
                                         opj_calloc(1, sizeof(opj_codestream_index_t));
    if (!cstr_index) {
        return NULL;
    }

    cstr_index->maxmarknum = 100;
    cstr_index->marknum = 0;
    cstr_index->marker = (opj_marker_info_t*)
                         opj_calloc(cstr_index->maxmarknum, sizeof(opj_marker_info_t));
    if (!cstr_index-> marker) {
        opj_free(cstr_index);
        return NULL;
    }

    cstr_index->tile_index = NULL;

    return cstr_index;
}
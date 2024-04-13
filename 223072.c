static OPJ_BOOL opj_j2k_add_mhmarker(opj_codestream_index_t *cstr_index,
                                     OPJ_UINT32 type, OPJ_OFF_T pos, OPJ_UINT32 len)
{
    assert(cstr_index != 00);

    /* expand the list? */
    if ((cstr_index->marknum + 1) > cstr_index->maxmarknum) {
        opj_marker_info_t *new_marker;
        cstr_index->maxmarknum = (OPJ_UINT32)(100 + (OPJ_FLOAT32)
                                              cstr_index->maxmarknum);
        new_marker = (opj_marker_info_t *) opj_realloc(cstr_index->marker,
                     cstr_index->maxmarknum * sizeof(opj_marker_info_t));
        if (! new_marker) {
            opj_free(cstr_index->marker);
            cstr_index->marker = NULL;
            cstr_index->maxmarknum = 0;
            cstr_index->marknum = 0;
            /* opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to add mh marker\n"); */
            return OPJ_FALSE;
        }
        cstr_index->marker = new_marker;
    }

    /* add the marker */
    cstr_index->marker[cstr_index->marknum].type = (OPJ_UINT16)type;
    cstr_index->marker[cstr_index->marknum].pos = (OPJ_INT32)pos;
    cstr_index->marker[cstr_index->marknum].len = (OPJ_INT32)len;
    cstr_index->marknum++;
    return OPJ_TRUE;
}
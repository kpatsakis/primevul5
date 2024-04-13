static OPJ_BOOL opj_j2k_add_tlmarker(OPJ_UINT32 tileno,
                                     opj_codestream_index_t *cstr_index, OPJ_UINT32 type, OPJ_OFF_T pos,
                                     OPJ_UINT32 len)
{
    assert(cstr_index != 00);
    assert(cstr_index->tile_index != 00);

    /* expand the list? */
    if ((cstr_index->tile_index[tileno].marknum + 1) >
            cstr_index->tile_index[tileno].maxmarknum) {
        opj_marker_info_t *new_marker;
        cstr_index->tile_index[tileno].maxmarknum = (OPJ_UINT32)(100 +
                (OPJ_FLOAT32) cstr_index->tile_index[tileno].maxmarknum);
        new_marker = (opj_marker_info_t *) opj_realloc(
                         cstr_index->tile_index[tileno].marker,
                         cstr_index->tile_index[tileno].maxmarknum * sizeof(opj_marker_info_t));
        if (! new_marker) {
            opj_free(cstr_index->tile_index[tileno].marker);
            cstr_index->tile_index[tileno].marker = NULL;
            cstr_index->tile_index[tileno].maxmarknum = 0;
            cstr_index->tile_index[tileno].marknum = 0;
            /* opj_event_msg(p_manager, EVT_ERROR, "Not enough memory to add tl marker\n"); */
            return OPJ_FALSE;
        }
        cstr_index->tile_index[tileno].marker = new_marker;
    }

    /* add the marker */
    cstr_index->tile_index[tileno].marker[cstr_index->tile_index[tileno].marknum].type
        = (OPJ_UINT16)type;
    cstr_index->tile_index[tileno].marker[cstr_index->tile_index[tileno].marknum].pos
        = (OPJ_INT32)pos;
    cstr_index->tile_index[tileno].marker[cstr_index->tile_index[tileno].marknum].len
        = (OPJ_INT32)len;
    cstr_index->tile_index[tileno].marknum++;

    if (type == J2K_MS_SOT) {
        OPJ_UINT32 l_current_tile_part = cstr_index->tile_index[tileno].current_tpsno;

        if (cstr_index->tile_index[tileno].tp_index) {
            cstr_index->tile_index[tileno].tp_index[l_current_tile_part].start_pos = pos;
        }

    }
    return OPJ_TRUE;
}
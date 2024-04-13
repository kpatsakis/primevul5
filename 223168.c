opj_tcd_marker_info_t* opj_tcd_marker_info_create(OPJ_BOOL need_PLT)
{
    opj_tcd_marker_info_t *l_tcd_marker_info =
        (opj_tcd_marker_info_t*) opj_calloc(1, sizeof(opj_tcd_marker_info_t));
    if (!l_tcd_marker_info) {
        return NULL;
    }

    l_tcd_marker_info->need_PLT = need_PLT;

    return l_tcd_marker_info;
}
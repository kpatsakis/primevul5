static const opj_dec_memory_marker_handler_t * opj_j2k_get_marker_handler(
    OPJ_UINT32 p_id)
{
    const opj_dec_memory_marker_handler_t *e;
    for (e = j2k_memory_marker_handler_tab; e->id != 0; ++e) {
        if (e->id == p_id) {
            break; /* we find a handler corresponding to the marker ID*/
        }
    }
    return e;
}
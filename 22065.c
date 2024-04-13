status_patterns(stream * s, pcl_state_t * pcs, pcl_data_storage_t storage)
{
    if (storage == 0) {
        int id = pcs->current_pattern_id;
        pcl_pattern_t *pptrn = pcl_pattern_get_pcl_uptrn(pcs, id);

        if ((pptrn != 0) && (pcs->pattern_type == pcl_pattern_user_defined)) {
            char id_string[6];

            gs_snprintf(id_string, sizeof(id_string), "%u", id);
            status_put_id(s, "IDLIST", id_string);
        }
    } else {
        int id;

        for (id = 0; id < (1L << 15) - 1; id++) {
            pcl_pattern_t *pptrn = pcl_pattern_get_pcl_uptrn(pcs, id);

            if (pptrn != 0) {
                char id_string[6];

                gs_snprintf(id_string, sizeof(id_string), "%u", id);
                status_put_id(s, "IDLIST", id_string);
            }
        }
    }
    status_end_id_list(s);
    return 0;
}
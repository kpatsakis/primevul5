status_macros(stream * s, pcl_state_t * pcs, pcl_data_storage_t storage)
{
    gs_const_string key;
    void *value;
    pl_dict_enum_t denum;

    if (storage == 0)
        return 0;               /* no "currently selected" macro */
    pl_dict_enum_begin(&pcs->macros, &denum);
    while (pl_dict_enum_next(&denum, &key, &value))
        if (((pcl_macro_t *) value)->storage & storage) {
            char id_string[6];

            gs_snprintf(id_string, sizeof(id_string), "%u", (key.data[0] << 8) + key.data[1]);
            status_put_id(s, "IDLIST", id_string);
        }
    status_end_id_list(s);
    return 0;
}
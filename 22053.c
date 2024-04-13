status_do_fonts(stream * s, pcl_state_t * pcs,
                pcl_data_storage_t storage, bool extended)
{
    gs_const_string key;
    void *value;
    pl_dict_enum_t denum;
    int res;

    pl_dict_enum_begin(&pcs->soft_fonts, &denum);
    while (pl_dict_enum_next(&denum, &key, &value)) {
        uint id = (key.data[0] << 8) + key.data[1];

        if ((((pl_font_t *) value)->storage & storage) != 0 ||
            (storage == 0 && pcs->font == (pl_font_t *) value)
            ) {
            res = status_put_font(s, pcs, id, id, (pl_font_t *) value,
                                  (storage != 0 ? -1 : pcs->font_selected),
                                  extended);
            if (res != 0)
                return res;
        }
    }
    return 0;
}
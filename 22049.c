status_check_symbol_set(pcl_state_t * pcs, pl_symbol_map_t * mapp,
                        pcl_data_storage_t storage)
{
    gs_const_string key;
    void *value;
    pl_dict_enum_t fenum;

    pl_dict_enum_begin(&pcs->soft_fonts, &fenum);
    while (pl_dict_enum_next(&fenum, &key, &value)) {
        pl_font_t *fp = (pl_font_t *) value;

        if (fp->storage != storage)
            continue;
        if (pcl_check_symbol_support(mapp->character_requirements,
                                     fp->character_complement))
            return true;
    }
    return false;
}
status_symbol_sets(stream * s, pcl_state_t * pcs, pcl_data_storage_t storage)
{
    gs_const_string key;
    void *value;
    pl_dict_enum_t denum;
    ushort *idlist;
    int nid;

    if (storage == 0)
        return 0;               /* no "currently selected" symbol set */

    /* Note carefully the meaning of this status inquiry.  First,
     * we return only symbol sets applicable to unbound fonts.  Second,
     * the "storage" value refers to the location of fonts. */

    /* total up built-in symbol sets, downloaded ones */
    nid = pl_dict_length(&pcs->soft_symbol_sets, false) +
        pl_dict_length(&pcs->built_in_symbol_sets, false);
    idlist = (ushort *) gs_alloc_bytes(pcs->memory, nid * sizeof(ushort),
                                       "status_symbol_sets(idlist)");
    if (idlist == NULL)
        return e_Memory;
    nid = 0;

    /* For each symbol set,
     *   for each font in appropriate storage,
     *     if the font supports that symbol set, list the symbol set
     *     and break (because we only need to find one such font). */

    /* NOTE: Temporarily chain soft, built-in symbol sets.  DON'T
     * exit this section without unchaining them. */
    pl_dict_set_parent(&pcs->soft_symbol_sets, &pcs->built_in_symbol_sets);
    pl_dict_enum_begin(&pcs->soft_symbol_sets, &denum);
    while (pl_dict_enum_next(&denum, &key, &value)) {
        pcl_symbol_set_t *ssp = (pcl_symbol_set_t *) value;
        pl_glyph_vocabulary_t gx;

        for (gx = plgv_MSL; gx < plgv_next; gx++)
            if (ssp->maps[gx] != NULL &&
                status_check_symbol_set(pcs, ssp->maps[gx], storage)) {
                nid = status_add_symbol_id(idlist, nid,
                                           (ssp->maps[gx]->id[0] << 8) +
                                           ssp->maps[gx]->id[1]);
                break;          /* one will suffice */
            }
    }
    pl_dict_set_parent(&pcs->soft_symbol_sets, NULL);
    /* Symbol sets are back to normal. */

    status_print_idlist(s, idlist, nid, "IDLIST");
    gs_free_object(pcs->memory, (void *)idlist, "status_symbol_sets(idlist)");
    return 0;
}
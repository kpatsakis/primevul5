static void opj_j2k_write_poc_in_memory(opj_j2k_t *p_j2k,
                                        OPJ_BYTE * p_data,
                                        OPJ_UINT32 * p_data_written,
                                        opj_event_mgr_t * p_manager
                                       )
{
    OPJ_UINT32 i;
    OPJ_BYTE * l_current_data = 00;
    OPJ_UINT32 l_nb_comp;
    OPJ_UINT32 l_nb_poc;
    OPJ_UINT32 l_poc_size;
    opj_image_t *l_image = 00;
    opj_tcp_t *l_tcp = 00;
    opj_tccp_t *l_tccp = 00;
    opj_poc_t *l_current_poc = 00;
    OPJ_UINT32 l_poc_room;

    /* preconditions */
    assert(p_j2k != 00);
    assert(p_manager != 00);

    OPJ_UNUSED(p_manager);

    l_tcp = &p_j2k->m_cp.tcps[p_j2k->m_current_tile_number];
    l_tccp = &l_tcp->tccps[0];
    l_image = p_j2k->m_private_image;
    l_nb_comp = l_image->numcomps;
    l_nb_poc = 1 + l_tcp->numpocs;

    if (l_nb_comp <= 256) {
        l_poc_room = 1;
    } else {
        l_poc_room = 2;
    }

    l_poc_size = 4 + (5 + 2 * l_poc_room) * l_nb_poc;

    l_current_data = p_data;

    opj_write_bytes(l_current_data, J2K_MS_POC,
                    2);                                   /* POC  */
    l_current_data += 2;

    opj_write_bytes(l_current_data, l_poc_size - 2,
                    2);                                 /* Lpoc */
    l_current_data += 2;

    l_current_poc =  l_tcp->pocs;
    for (i = 0; i < l_nb_poc; ++i) {
        opj_write_bytes(l_current_data, l_current_poc->resno0,
                        1);                                /* RSpoc_i */
        ++l_current_data;

        opj_write_bytes(l_current_data, l_current_poc->compno0,
                        l_poc_room);              /* CSpoc_i */
        l_current_data += l_poc_room;

        opj_write_bytes(l_current_data, l_current_poc->layno1,
                        2);                                /* LYEpoc_i */
        l_current_data += 2;

        opj_write_bytes(l_current_data, l_current_poc->resno1,
                        1);                                /* REpoc_i */
        ++l_current_data;

        opj_write_bytes(l_current_data, l_current_poc->compno1,
                        l_poc_room);              /* CEpoc_i */
        l_current_data += l_poc_room;

        opj_write_bytes(l_current_data, (OPJ_UINT32)l_current_poc->prg,
                        1);   /* Ppoc_i */
        ++l_current_data;

        /* change the value of the max layer according to the actual number of layers in the file, components and resolutions*/
        l_current_poc->layno1 = (OPJ_UINT32)opj_int_min((OPJ_INT32)
                                l_current_poc->layno1, (OPJ_INT32)l_tcp->numlayers);
        l_current_poc->resno1 = (OPJ_UINT32)opj_int_min((OPJ_INT32)
                                l_current_poc->resno1, (OPJ_INT32)l_tccp->numresolutions);
        l_current_poc->compno1 = (OPJ_UINT32)opj_int_min((OPJ_INT32)
                                 l_current_poc->compno1, (OPJ_INT32)l_nb_comp);

        ++l_current_poc;
    }

    *p_data_written = l_poc_size;
}
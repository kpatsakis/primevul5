static OPJ_BOOL opj_j2k_check_poc_val(const opj_poc_t *p_pocs,
                                      OPJ_UINT32 tileno,
                                      OPJ_UINT32 p_nb_pocs,
                                      OPJ_UINT32 p_nb_resolutions,
                                      OPJ_UINT32 p_num_comps,
                                      OPJ_UINT32 p_num_layers,
                                      opj_event_mgr_t * p_manager)
{
    OPJ_UINT32* packet_array;
    OPJ_UINT32 index, resno, compno, layno;
    OPJ_UINT32 i;
    OPJ_UINT32 step_c = 1;
    OPJ_UINT32 step_r = p_num_comps * step_c;
    OPJ_UINT32 step_l = p_nb_resolutions * step_r;
    OPJ_BOOL loss = OPJ_FALSE;

    assert(p_nb_pocs > 0);

    packet_array = (OPJ_UINT32*) opj_calloc(step_l * p_num_layers,
                                            sizeof(OPJ_UINT32));
    if (packet_array == 00) {
        opj_event_msg(p_manager, EVT_ERROR,
                      "Not enough memory for checking the poc values.\n");
        return OPJ_FALSE;
    }

    /* iterate through all the pocs that match our tile of interest. */
    for (i = 0; i < p_nb_pocs; ++i) {
        const opj_poc_t *poc = &p_pocs[i];
        if (tileno + 1 == poc->tile) {
            index = step_r * poc->resno0;

            /* take each resolution for each poc */
            for (resno = poc->resno0 ;
                    resno < opj_uint_min(poc->resno1, p_nb_resolutions); ++resno) {
                OPJ_UINT32 res_index = index + poc->compno0 * step_c;

                /* take each comp of each resolution for each poc */
                for (compno = poc->compno0 ;
                        compno < opj_uint_min(poc->compno1, p_num_comps); ++compno) {
                    /* The layer index always starts at zero for every progression. */
                    const OPJ_UINT32 layno0 = 0;
                    OPJ_UINT32 comp_index = res_index + layno0 * step_l;

                    /* and finally take each layer of each res of ... */
                    for (layno = layno0; layno < opj_uint_min(poc->layno1, p_num_layers);
                            ++layno) {
                        packet_array[comp_index] = 1;
                        comp_index += step_l;
                    }

                    res_index += step_c;
                }

                index += step_r;
            }
        }
    }

    index = 0;
    for (layno = 0; layno < p_num_layers ; ++layno) {
        for (resno = 0; resno < p_nb_resolutions; ++resno) {
            for (compno = 0; compno < p_num_comps; ++compno) {
                loss |= (packet_array[index] != 1);
#ifdef DEBUG_VERBOSE
                if (packet_array[index] != 1) {
                    fprintf(stderr,
                            "Missing packet in POC: layno=%d resno=%d compno=%d\n",
                            layno, resno, compno);
                }
#endif
                index += step_c;
            }
        }
    }

    if (loss) {
        opj_event_msg(p_manager, EVT_ERROR, "Missing packets possible loss of data\n");
    }

    opj_free(packet_array);

    return !loss;
}
static OPJ_UINT32 opj_j2k_get_num_tp(opj_cp_t *cp, OPJ_UINT32 pino,
                                     OPJ_UINT32 tileno)
{
    const OPJ_CHAR *prog = 00;
    OPJ_INT32 i;
    OPJ_UINT32 tpnum = 1;
    opj_tcp_t *tcp = 00;
    opj_poc_t * l_current_poc = 00;

    /*  preconditions */
    assert(tileno < (cp->tw * cp->th));
    assert(pino < (cp->tcps[tileno].numpocs + 1));

    /* get the given tile coding parameter */
    tcp = &cp->tcps[tileno];
    assert(tcp != 00);

    l_current_poc = &(tcp->pocs[pino]);
    assert(l_current_poc != 0);

    /* get the progression order as a character string */
    prog = opj_j2k_convert_progression_order(tcp->prg);
    assert(strlen(prog) > 0);

    if (cp->m_specific_param.m_enc.m_tp_on == 1) {
        for (i = 0; i < 4; ++i) {
            switch (prog[i]) {
            /* component wise */
            case 'C':
                tpnum *= l_current_poc->compE;
                break;
            /* resolution wise */
            case 'R':
                tpnum *= l_current_poc->resE;
                break;
            /* precinct wise */
            case 'P':
                tpnum *= l_current_poc->prcE;
                break;
            /* layer wise */
            case 'L':
                tpnum *= l_current_poc->layE;
                break;
            }
            /* whould we split here ? */
            if (cp->m_specific_param.m_enc.m_tp_flag == prog[i]) {
                cp->m_specific_param.m_enc.m_tp_pos = i;
                break;
            }
        }
    } else {
        tpnum = 1;
    }

    return tpnum;
}
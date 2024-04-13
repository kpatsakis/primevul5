static OPJ_BOOL opj_tcd_t1_decode(opj_tcd_t *p_tcd, opj_event_mgr_t *p_manager)
{
    OPJ_UINT32 compno;
    opj_tcd_tile_t * l_tile = p_tcd->tcd_image->tiles;
    opj_tcd_tilecomp_t* l_tile_comp = l_tile->comps;
    opj_tccp_t * l_tccp = p_tcd->tcp->tccps;
    volatile OPJ_BOOL ret = OPJ_TRUE;
    OPJ_BOOL check_pterm = OPJ_FALSE;
    opj_mutex_t* p_manager_mutex = NULL;

    p_manager_mutex = opj_mutex_create();

    /* Only enable PTERM check if we decode all layers */
    if (p_tcd->tcp->num_layers_to_decode == p_tcd->tcp->numlayers &&
            (l_tccp->cblksty & J2K_CCP_CBLKSTY_PTERM) != 0) {
        check_pterm = OPJ_TRUE;
    }

    for (compno = 0; compno < l_tile->numcomps;
            ++compno, ++l_tile_comp, ++l_tccp) {
        if (p_tcd->used_component != NULL && !p_tcd->used_component[compno]) {
            continue;
        }

        opj_t1_decode_cblks(p_tcd, &ret, l_tile_comp, l_tccp,
                            p_manager, p_manager_mutex, check_pterm);
        if (!ret) {
            break;
        }
    }

    opj_thread_pool_wait_completion(p_tcd->thread_pool, 0);
    if (p_manager_mutex) {
        opj_mutex_destroy(p_manager_mutex);
    }
    return ret;
}
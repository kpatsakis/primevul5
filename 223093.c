static void opj_j2k_set_cinema_parameters(opj_cparameters_t *parameters,
        opj_image_t *image, opj_event_mgr_t *p_manager)
{
    /* Configure cinema parameters */
    int i;

    /* No tiling */
    parameters->tile_size_on = OPJ_FALSE;
    parameters->cp_tdx = 1;
    parameters->cp_tdy = 1;

    /* One tile part for each component */
    parameters->tp_flag = 'C';
    parameters->tp_on = 1;

    /* Tile and Image shall be at (0,0) */
    parameters->cp_tx0 = 0;
    parameters->cp_ty0 = 0;
    parameters->image_offset_x0 = 0;
    parameters->image_offset_y0 = 0;

    /* Codeblock size= 32*32 */
    parameters->cblockw_init = 32;
    parameters->cblockh_init = 32;

    /* Codeblock style: no mode switch enabled */
    parameters->mode = 0;

    /* No ROI */
    parameters->roi_compno = -1;

    /* No subsampling */
    parameters->subsampling_dx = 1;
    parameters->subsampling_dy = 1;

    /* 9-7 transform */
    parameters->irreversible = 1;

    /* Number of layers */
    if (parameters->tcp_numlayers > 1) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "JPEG 2000 Profile-3 and 4 (2k/4k dc profile) requires:\n"
                      "1 single quality layer"
                      "-> Number of layers forced to 1 (rather than %d)\n"
                      "-> Rate of the last layer (%3.1f) will be used",
                      parameters->tcp_numlayers,
                      parameters->tcp_rates[parameters->tcp_numlayers - 1]);
        parameters->tcp_rates[0] = parameters->tcp_rates[parameters->tcp_numlayers - 1];
        parameters->tcp_numlayers = 1;
    }

    /* Resolution levels */
    switch (parameters->rsiz) {
    case OPJ_PROFILE_CINEMA_2K:
        if (parameters->numresolution > 6) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "JPEG 2000 Profile-3 (2k dc profile) requires:\n"
                          "Number of decomposition levels <= 5\n"
                          "-> Number of decomposition levels forced to 5 (rather than %d)\n",
                          parameters->numresolution + 1);
            parameters->numresolution = 6;
        }
        break;
    case OPJ_PROFILE_CINEMA_4K:
        if (parameters->numresolution < 2) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "JPEG 2000 Profile-4 (4k dc profile) requires:\n"
                          "Number of decomposition levels >= 1 && <= 6\n"
                          "-> Number of decomposition levels forced to 1 (rather than %d)\n",
                          parameters->numresolution + 1);
            parameters->numresolution = 1;
        } else if (parameters->numresolution > 7) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "JPEG 2000 Profile-4 (4k dc profile) requires:\n"
                          "Number of decomposition levels >= 1 && <= 6\n"
                          "-> Number of decomposition levels forced to 6 (rather than %d)\n",
                          parameters->numresolution + 1);
            parameters->numresolution = 7;
        }
        break;
    default :
        break;
    }

    /* Precincts */
    parameters->csty |= J2K_CP_CSTY_PRT;
    if (parameters->numresolution == 1) {
        parameters->res_spec = 1;
        parameters->prcw_init[0] = 128;
        parameters->prch_init[0] = 128;
    } else {
        parameters->res_spec = parameters->numresolution - 1;
        for (i = 0; i < parameters->res_spec; i++) {
            parameters->prcw_init[i] = 256;
            parameters->prch_init[i] = 256;
        }
    }

    /* The progression order shall be CPRL */
    parameters->prog_order = OPJ_CPRL;

    /* Progression order changes for 4K, disallowed for 2K */
    if (parameters->rsiz == OPJ_PROFILE_CINEMA_4K) {
        parameters->numpocs = (OPJ_UINT32)opj_j2k_initialise_4K_poc(parameters->POC,
                              parameters->numresolution);
    } else {
        parameters->numpocs = 0;
    }

    /* Limited bit-rate */
    parameters->cp_disto_alloc = 1;
    if (parameters->max_cs_size <= 0) {
        /* No rate has been introduced, 24 fps is assumed */
        parameters->max_cs_size = OPJ_CINEMA_24_CS;
        opj_event_msg(p_manager, EVT_WARNING,
                      "JPEG 2000 Profile-3 and 4 (2k/4k dc profile) requires:\n"
                      "Maximum 1302083 compressed bytes @ 24fps\n"
                      "As no rate has been given, this limit will be used.\n");
    } else if (parameters->max_cs_size > OPJ_CINEMA_24_CS) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "JPEG 2000 Profile-3 and 4 (2k/4k dc profile) requires:\n"
                      "Maximum 1302083 compressed bytes @ 24fps\n"
                      "-> Specified rate exceeds this limit. Rate will be forced to 1302083 bytes.\n");
        parameters->max_cs_size = OPJ_CINEMA_24_CS;
    }

    if (parameters->max_comp_size <= 0) {
        /* No rate has been introduced, 24 fps is assumed */
        parameters->max_comp_size = OPJ_CINEMA_24_COMP;
        opj_event_msg(p_manager, EVT_WARNING,
                      "JPEG 2000 Profile-3 and 4 (2k/4k dc profile) requires:\n"
                      "Maximum 1041666 compressed bytes @ 24fps\n"
                      "As no rate has been given, this limit will be used.\n");
    } else if (parameters->max_comp_size > OPJ_CINEMA_24_COMP) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "JPEG 2000 Profile-3 and 4 (2k/4k dc profile) requires:\n"
                      "Maximum 1041666 compressed bytes @ 24fps\n"
                      "-> Specified rate exceeds this limit. Rate will be forced to 1041666 bytes.\n");
        parameters->max_comp_size = OPJ_CINEMA_24_COMP;
    }

    parameters->tcp_rates[0] = (OPJ_FLOAT32)(image->numcomps * image->comps[0].w *
                               image->comps[0].h * image->comps[0].prec) /
                               (OPJ_FLOAT32)(((OPJ_UINT32)parameters->max_cs_size) * 8 * image->comps[0].dx *
                                       image->comps[0].dy);

}
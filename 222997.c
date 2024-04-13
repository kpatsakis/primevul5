static void opj_j2k_set_imf_parameters(opj_cparameters_t *parameters,
                                       opj_image_t *image, opj_event_mgr_t *p_manager)
{
    const OPJ_UINT16 rsiz = parameters->rsiz;
    const OPJ_UINT16 profile = OPJ_GET_IMF_PROFILE(rsiz);

    OPJ_UNUSED(p_manager);

    /* Override defaults set by opj_set_default_encoder_parameters */
    if (parameters->cblockw_init == OPJ_COMP_PARAM_DEFAULT_CBLOCKW &&
            parameters->cblockh_init == OPJ_COMP_PARAM_DEFAULT_CBLOCKH) {
        parameters->cblockw_init = 32;
        parameters->cblockh_init = 32;
    }

    /* One tile part for each component */
    parameters->tp_flag = 'C';
    parameters->tp_on = 1;

    if (parameters->prog_order == OPJ_COMP_PARAM_DEFAULT_PROG_ORDER) {
        parameters->prog_order = OPJ_CPRL;
    }

    if (profile == OPJ_PROFILE_IMF_2K ||
            profile == OPJ_PROFILE_IMF_4K ||
            profile == OPJ_PROFILE_IMF_8K) {
        /* 9-7 transform */
        parameters->irreversible = 1;
    }

    /* Adjust the number of resolutions if set to its defaults */
    if (parameters->numresolution == OPJ_COMP_PARAM_DEFAULT_NUMRESOLUTION &&
            image->x0 == 0 &&
            image->y0 == 0) {
        const int max_NL = opj_j2k_get_imf_max_NL(parameters, image);
        if (max_NL >= 0 && parameters->numresolution > max_NL) {
            parameters->numresolution = max_NL + 1;
        }

        /* Note: below is generic logic */
        if (!parameters->tile_size_on) {
            while (parameters->numresolution > 0) {
                if (image->x1 < (1U << ((OPJ_UINT32)parameters->numresolution - 1U))) {
                    parameters->numresolution --;
                    continue;
                }
                if (image->y1 < (1U << ((OPJ_UINT32)parameters->numresolution - 1U))) {
                    parameters->numresolution --;
                    continue;
                }
                break;
            }
        }
    }

    /* Set defaults precincts */
    if (parameters->csty == 0) {
        parameters->csty |= J2K_CP_CSTY_PRT;
        if (parameters->numresolution == 1) {
            parameters->res_spec = 1;
            parameters->prcw_init[0] = 128;
            parameters->prch_init[0] = 128;
        } else {
            int i;
            parameters->res_spec = parameters->numresolution - 1;
            for (i = 0; i < parameters->res_spec; i++) {
                parameters->prcw_init[i] = 256;
                parameters->prch_init[i] = 256;
            }
        }
    }
}
static OPJ_BOOL opj_j2k_is_imf_compliant(opj_cparameters_t *parameters,
        opj_image_t *image,
        opj_event_mgr_t *p_manager)
{
    OPJ_UINT32 i;
    const OPJ_UINT16 rsiz = parameters->rsiz;
    const OPJ_UINT16 profile = OPJ_GET_IMF_PROFILE(rsiz);
    const OPJ_UINT16 mainlevel = OPJ_GET_IMF_MAINLEVEL(rsiz);
    const OPJ_UINT16 sublevel = OPJ_GET_IMF_SUBLEVEL(rsiz);
    const int NL = parameters->numresolution - 1;
    const OPJ_UINT32 XTsiz = parameters->tile_size_on ? (OPJ_UINT32)
                             parameters->cp_tdx : image->x1;
    OPJ_BOOL ret = OPJ_TRUE;

    /* Validate mainlevel */
    if (mainlevel > OPJ_IMF_MAINLEVEL_MAX) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "IMF profile require mainlevel <= 11.\n"
                      "-> %d is thus not compliant\n"
                      "-> Non-IMF codestream will be generated\n",
                      mainlevel);
        ret = OPJ_FALSE;
    }

    /* Validate sublevel */
    assert(sizeof(tabMaxSubLevelFromMainLevel) ==
           (OPJ_IMF_MAINLEVEL_MAX + 1) * sizeof(tabMaxSubLevelFromMainLevel[0]));
    if (sublevel > tabMaxSubLevelFromMainLevel[mainlevel]) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "IMF profile require sublevel <= %d for mainlevel = %d.\n"
                      "-> %d is thus not compliant\n"
                      "-> Non-IMF codestream will be generated\n",
                      tabMaxSubLevelFromMainLevel[mainlevel],
                      mainlevel,
                      sublevel);
        ret = OPJ_FALSE;
    }

    /* Number of components */
    if (image->numcomps > 3) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "IMF profiles require at most 3 components.\n"
                      "-> Number of components of input image (%d) is not compliant\n"
                      "-> Non-IMF codestream will be generated\n",
                      image->numcomps);
        ret = OPJ_FALSE;
    }

    if (image->x0 != 0 || image->y0 != 0) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "IMF profiles require image origin to be at 0,0.\n"
                      "-> %d,%d is not compliant\n"
                      "-> Non-IMF codestream will be generated\n",
                      image->x0, image->y0 != 0);
        ret = OPJ_FALSE;
    }

    if (parameters->cp_tx0 != 0 || parameters->cp_ty0 != 0) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "IMF profiles require tile origin to be at 0,0.\n"
                      "-> %d,%d is not compliant\n"
                      "-> Non-IMF codestream will be generated\n",
                      parameters->cp_tx0, parameters->cp_ty0);
        ret = OPJ_FALSE;
    }

    if (parameters->tile_size_on) {
        if (profile == OPJ_PROFILE_IMF_2K ||
                profile == OPJ_PROFILE_IMF_4K ||
                profile == OPJ_PROFILE_IMF_8K) {
            if ((OPJ_UINT32)parameters->cp_tdx < image->x1 ||
                    (OPJ_UINT32)parameters->cp_tdy < image->y1) {
                opj_event_msg(p_manager, EVT_WARNING,
                              "IMF 2K/4K/8K single tile profiles require tile to be greater or equal to image size.\n"
                              "-> %d,%d is lesser than %d,%d\n"
                              "-> Non-IMF codestream will be generated\n",
                              parameters->cp_tdx,
                              parameters->cp_tdy,
                              image->x1,
                              image->y1);
                ret = OPJ_FALSE;
            }
        } else {
            if ((OPJ_UINT32)parameters->cp_tdx >= image->x1 &&
                    (OPJ_UINT32)parameters->cp_tdy >= image->y1) {
                /* ok */
            } else if (parameters->cp_tdx == 1024 &&
                       parameters->cp_tdy == 1024) {
                /* ok */
            } else if (parameters->cp_tdx == 2048 &&
                       parameters->cp_tdy == 2048 &&
                       (profile == OPJ_PROFILE_IMF_4K ||
                        profile == OPJ_PROFILE_IMF_8K)) {
                /* ok */
            } else if (parameters->cp_tdx == 4096 &&
                       parameters->cp_tdy == 4096 &&
                       profile == OPJ_PROFILE_IMF_8K) {
                /* ok */
            } else {
                opj_event_msg(p_manager, EVT_WARNING,
                              "IMF 2K_R/4K_R/8K_R single/multiple tile profiles "
                              "require tile to be greater or equal to image size,\n"
                              "or to be (1024,1024), or (2048,2048) for 4K_R/8K_R "
                              "or (4096,4096) for 8K_R.\n"
                              "-> %d,%d is non conformant\n"
                              "-> Non-IMF codestream will be generated\n",
                              parameters->cp_tdx,
                              parameters->cp_tdy);
                ret = OPJ_FALSE;
            }
        }
    }

    /* Bitdepth */
    for (i = 0; i < image->numcomps; i++) {
        if (!(image->comps[i].bpp >= 8 && image->comps[i].bpp <= 16) ||
                (image->comps[i].sgnd)) {
            char signed_str[] = "signed";
            char unsigned_str[] = "unsigned";
            char *tmp_str = image->comps[i].sgnd ? signed_str : unsigned_str;
            opj_event_msg(p_manager, EVT_WARNING,
                          "IMF profiles require precision of each component to b in [8-16] bits unsigned"
                          "-> At least component %d of input image (%d bits, %s) is not compliant\n"
                          "-> Non-IMF codestream will be generated\n",
                          i, image->comps[i].bpp, tmp_str);
            ret = OPJ_FALSE;
        }
    }

    /* Sub-sampling */
    for (i = 0; i < image->numcomps; i++) {
        if (i == 0 && image->comps[i].dx != 1) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "IMF profiles require XRSiz1 == 1. Here it is set to %d.\n"
                          "-> Non-IMF codestream will be generated\n",
                          image->comps[i].dx);
            ret = OPJ_FALSE;
        }
        if (i == 1 && image->comps[i].dx != 1 && image->comps[i].dx != 2) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "IMF profiles require XRSiz2 == 1 or 2. Here it is set to %d.\n"
                          "-> Non-IMF codestream will be generated\n",
                          image->comps[i].dx);
            ret = OPJ_FALSE;
        }
        if (i > 1 && image->comps[i].dx != image->comps[i - 1].dx) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "IMF profiles require XRSiz%d to be the same as XRSiz2. "
                          "Here it is set to %d instead of %d.\n"
                          "-> Non-IMF codestream will be generated\n",
                          i + 1, image->comps[i].dx, image->comps[i - 1].dx);
            ret = OPJ_FALSE;
        }
        if (image->comps[i].dy != 1) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "IMF profiles require YRsiz == 1. "
                          "Here it is set to %d for component i.\n"
                          "-> Non-IMF codestream will be generated\n",
                          image->comps[i].dy, i);
            ret = OPJ_FALSE;
        }
    }

    /* Image size */
    switch (profile) {
    case OPJ_PROFILE_IMF_2K:
    case OPJ_PROFILE_IMF_2K_R:
        if (((image->comps[0].w > 2048) | (image->comps[0].h > 1556))) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "IMF 2K/2K_R profile require:\n"
                          "width <= 2048 and height <= 1556\n"
                          "-> Input image size %d x %d is not compliant\n"
                          "-> Non-IMF codestream will be generated\n",
                          image->comps[0].w, image->comps[0].h);
            ret = OPJ_FALSE;
        }
        break;
    case OPJ_PROFILE_IMF_4K:
    case OPJ_PROFILE_IMF_4K_R:
        if (((image->comps[0].w > 4096) | (image->comps[0].h > 3112))) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "IMF 4K/4K_R profile require:\n"
                          "width <= 4096 and height <= 3112\n"
                          "-> Input image size %d x %d is not compliant\n"
                          "-> Non-IMF codestream will be generated\n",
                          image->comps[0].w, image->comps[0].h);
            ret = OPJ_FALSE;
        }
        break;
    case OPJ_PROFILE_IMF_8K:
    case OPJ_PROFILE_IMF_8K_R:
        if (((image->comps[0].w > 8192) | (image->comps[0].h > 6224))) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "IMF 8K/8K_R profile require:\n"
                          "width <= 8192 and height <= 6224\n"
                          "-> Input image size %d x %d is not compliant\n"
                          "-> Non-IMF codestream will be generated\n",
                          image->comps[0].w, image->comps[0].h);
            ret = OPJ_FALSE;
        }
        break;
    default :
        assert(0);
        return OPJ_FALSE;
    }

    if (parameters->roi_compno != -1) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "IMF profile forbid RGN / region of interest marker.\n"
                      "-> Compression parameters specify a ROI\n"
                      "-> Non-IMF codestream will be generated\n");
        ret = OPJ_FALSE;
    }

    if (parameters->cblockw_init != 32 || parameters->cblockh_init != 32) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "IMF profile require code block size to be 32x32.\n"
                      "-> Compression parameters set it to %dx%d.\n"
                      "-> Non-IMF codestream will be generated\n",
                      parameters->cblockw_init,
                      parameters->cblockh_init);
        ret = OPJ_FALSE;
    }

    if (parameters->prog_order != OPJ_CPRL) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "IMF profile require progression order to be CPRL.\n"
                      "-> Compression parameters set it to %d.\n"
                      "-> Non-IMF codestream will be generated\n",
                      parameters->prog_order);
        ret = OPJ_FALSE;
    }

    if (parameters->numpocs != 0) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "IMF profile forbid POC markers.\n"
                      "-> Compression parameters set %d POC.\n"
                      "-> Non-IMF codestream will be generated\n",
                      parameters->numpocs);
        ret = OPJ_FALSE;
    }

    /* Codeblock style: no mode switch enabled */
    if (parameters->mode != 0) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "IMF profile forbid mode switch in code block style.\n"
                      "-> Compression parameters set code block style to %d.\n"
                      "-> Non-IMF codestream will be generated\n",
                      parameters->mode);
        ret = OPJ_FALSE;
    }

    if (profile == OPJ_PROFILE_IMF_2K ||
            profile == OPJ_PROFILE_IMF_4K ||
            profile == OPJ_PROFILE_IMF_8K) {
        /* Expect 9-7 transform */
        if (parameters->irreversible != 1) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "IMF 2K/4K/8K profiles require 9-7 Irreversible Transform.\n"
                          "-> Compression parameters set it to reversible.\n"
                          "-> Non-IMF codestream will be generated\n");
            ret = OPJ_FALSE;
        }
    } else {
        /* Expect 5-3 transform */
        if (parameters->irreversible != 0) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "IMF 2K/4K/8K profiles require 5-3 reversible Transform.\n"
                          "-> Compression parameters set it to irreversible.\n"
                          "-> Non-IMF codestream will be generated\n");
            ret = OPJ_FALSE;
        }
    }

    /* Number of layers */
    if (parameters->tcp_numlayers != 1) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "IMF 2K/4K/8K profiles require 1 single quality layer.\n"
                      "-> Number of layers is %d.\n"
                      "-> Non-IMF codestream will be generated\n",
                      parameters->tcp_numlayers);
        ret = OPJ_FALSE;
    }

    /* Decomposition levels */
    switch (profile) {
    case OPJ_PROFILE_IMF_2K:
        if (!(NL >= 1 && NL <= 5)) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "IMF 2K profile requires 1 <= NL <= 5:\n"
                          "-> Number of decomposition levels is %d.\n"
                          "-> Non-IMF codestream will be generated\n",
                          NL);
            ret = OPJ_FALSE;
        }
        break;
    case OPJ_PROFILE_IMF_4K:
        if (!(NL >= 1 && NL <= 6)) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "IMF 4K profile requires 1 <= NL <= 6:\n"
                          "-> Number of decomposition levels is %d.\n"
                          "-> Non-IMF codestream will be generated\n",
                          NL);
            ret = OPJ_FALSE;
        }
        break;
    case OPJ_PROFILE_IMF_8K:
        if (!(NL >= 1 && NL <= 7)) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "IMF 8K profile requires 1 <= NL <= 7:\n"
                          "-> Number of decomposition levels is %d.\n"
                          "-> Non-IMF codestream will be generated\n",
                          NL);
            ret = OPJ_FALSE;
        }
        break;
    case OPJ_PROFILE_IMF_2K_R: {
        if (XTsiz >= 2048) {
            if (!(NL >= 1 && NL <= 5)) {
                opj_event_msg(p_manager, EVT_WARNING,
                              "IMF 2K_R profile requires 1 <= NL <= 5 for XTsiz >= 2048:\n"
                              "-> Number of decomposition levels is %d.\n"
                              "-> Non-IMF codestream will be generated\n",
                              NL);
                ret = OPJ_FALSE;
            }
        } else if (XTsiz >= 1024) {
            if (!(NL >= 1 && NL <= 4)) {
                opj_event_msg(p_manager, EVT_WARNING,
                              "IMF 2K_R profile requires 1 <= NL <= 4 for XTsiz in [1024,2048[:\n"
                              "-> Number of decomposition levels is %d.\n"
                              "-> Non-IMF codestream will be generated\n",
                              NL);
                ret = OPJ_FALSE;
            }
        }
        break;
    }
    case OPJ_PROFILE_IMF_4K_R: {
        if (XTsiz >= 4096) {
            if (!(NL >= 1 && NL <= 6)) {
                opj_event_msg(p_manager, EVT_WARNING,
                              "IMF 4K_R profile requires 1 <= NL <= 6 for XTsiz >= 4096:\n"
                              "-> Number of decomposition levels is %d.\n"
                              "-> Non-IMF codestream will be generated\n",
                              NL);
                ret = OPJ_FALSE;
            }
        } else if (XTsiz >= 2048) {
            if (!(NL >= 1 && NL <= 5)) {
                opj_event_msg(p_manager, EVT_WARNING,
                              "IMF 4K_R profile requires 1 <= NL <= 5 for XTsiz in [2048,4096[:\n"
                              "-> Number of decomposition levels is %d.\n"
                              "-> Non-IMF codestream will be generated\n",
                              NL);
                ret = OPJ_FALSE;
            }
        } else if (XTsiz >= 1024) {
            if (!(NL >= 1 && NL <= 4)) {
                opj_event_msg(p_manager, EVT_WARNING,
                              "IMF 4K_R profile requires 1 <= NL <= 4 for XTsiz in [1024,2048[:\n"
                              "-> Number of decomposition levels is %d.\n"
                              "-> Non-IMF codestream will be generated\n",
                              NL);
                ret = OPJ_FALSE;
            }
        }
        break;
    }
    case OPJ_PROFILE_IMF_8K_R: {
        if (XTsiz >= 8192) {
            if (!(NL >= 1 && NL <= 7)) {
                opj_event_msg(p_manager, EVT_WARNING,
                              "IMF 4K_R profile requires 1 <= NL <= 7 for XTsiz >= 8192:\n"
                              "-> Number of decomposition levels is %d.\n"
                              "-> Non-IMF codestream will be generated\n",
                              NL);
                ret = OPJ_FALSE;
            }
        } else if (XTsiz >= 4096) {
            if (!(NL >= 1 && NL <= 6)) {
                opj_event_msg(p_manager, EVT_WARNING,
                              "IMF 4K_R profile requires 1 <= NL <= 6 for XTsiz in [4096,8192[:\n"
                              "-> Number of decomposition levels is %d.\n"
                              "-> Non-IMF codestream will be generated\n",
                              NL);
                ret = OPJ_FALSE;
            }
        } else if (XTsiz >= 2048) {
            if (!(NL >= 1 && NL <= 5)) {
                opj_event_msg(p_manager, EVT_WARNING,
                              "IMF 4K_R profile requires 1 <= NL <= 5 for XTsiz in [2048,4096[:\n"
                              "-> Number of decomposition levels is %d.\n"
                              "-> Non-IMF codestream will be generated\n",
                              NL);
                ret = OPJ_FALSE;
            }
        } else if (XTsiz >= 1024) {
            if (!(NL >= 1 && NL <= 4)) {
                opj_event_msg(p_manager, EVT_WARNING,
                              "IMF 4K_R profile requires 1 <= NL <= 4 for XTsiz in [1024,2048[:\n"
                              "-> Number of decomposition levels is %d.\n"
                              "-> Non-IMF codestream will be generated\n",
                              NL);
                ret = OPJ_FALSE;
            }
        }
        break;
    }
    default:
        break;
    }

    if (parameters->numresolution == 1) {
        if (parameters->res_spec != 1 ||
                parameters->prcw_init[0] != 128 ||
                parameters->prch_init[0] != 128) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "IMF profiles require PPx = PPy = 7 for NLLL band, else 8.\n"
                          "-> Supplied values are different from that.\n"
                          "-> Non-IMF codestream will be generated\n",
                          NL);
            ret = OPJ_FALSE;
        }
    } else {
        int i;
        for (i = 0; i < parameters->res_spec; i++) {
            if (parameters->prcw_init[i] != 256 ||
                    parameters->prch_init[i] != 256) {
                opj_event_msg(p_manager, EVT_WARNING,
                              "IMF profiles require PPx = PPy = 7 for NLLL band, else 8.\n"
                              "-> Supplied values are different from that.\n"
                              "-> Non-IMF codestream will be generated\n",
                              NL);
                ret = OPJ_FALSE;
            }
        }
    }

    return ret;
}
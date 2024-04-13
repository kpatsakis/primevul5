static int opj_j2k_get_imf_max_NL(opj_cparameters_t *parameters,
                                  opj_image_t *image)
{
    /* Decomposition levels */
    const OPJ_UINT16 rsiz = parameters->rsiz;
    const OPJ_UINT16 profile = OPJ_GET_IMF_PROFILE(rsiz);
    const OPJ_UINT32 XTsiz = parameters->tile_size_on ? (OPJ_UINT32)
                             parameters->cp_tdx : image->x1;
    switch (profile) {
    case OPJ_PROFILE_IMF_2K:
        return 5;
    case OPJ_PROFILE_IMF_4K:
        return 6;
    case OPJ_PROFILE_IMF_8K:
        return 7;
    case OPJ_PROFILE_IMF_2K_R: {
        if (XTsiz >= 2048) {
            return 5;
        } else if (XTsiz >= 1024) {
            return 4;
        }
        break;
    }
    case OPJ_PROFILE_IMF_4K_R: {
        if (XTsiz >= 4096) {
            return 6;
        } else if (XTsiz >= 2048) {
            return 5;
        } else if (XTsiz >= 1024) {
            return 4;
        }
        break;
    }
    case OPJ_PROFILE_IMF_8K_R: {
        if (XTsiz >= 8192) {
            return 7;
        } else if (XTsiz >= 4096) {
            return 6;
        } else if (XTsiz >= 2048) {
            return 5;
        } else if (XTsiz >= 1024) {
            return 4;
        }
        break;
    }
    default:
        break;
    }
    return -1;
}
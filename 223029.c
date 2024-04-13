static OPJ_BOOL opj_j2k_is_cinema_compliant(opj_image_t *image, OPJ_UINT16 rsiz,
        opj_event_mgr_t *p_manager)
{
    OPJ_UINT32 i;

    /* Number of components */
    if (image->numcomps != 3) {
        opj_event_msg(p_manager, EVT_WARNING,
                      "JPEG 2000 Profile-3 (2k dc profile) requires:\n"
                      "3 components"
                      "-> Number of components of input image (%d) is not compliant\n"
                      "-> Non-profile-3 codestream will be generated\n",
                      image->numcomps);
        return OPJ_FALSE;
    }

    /* Bitdepth */
    for (i = 0; i < image->numcomps; i++) {
        if ((image->comps[i].bpp != 12) | (image->comps[i].sgnd)) {
            char signed_str[] = "signed";
            char unsigned_str[] = "unsigned";
            char *tmp_str = image->comps[i].sgnd ? signed_str : unsigned_str;
            opj_event_msg(p_manager, EVT_WARNING,
                          "JPEG 2000 Profile-3 (2k dc profile) requires:\n"
                          "Precision of each component shall be 12 bits unsigned"
                          "-> At least component %d of input image (%d bits, %s) is not compliant\n"
                          "-> Non-profile-3 codestream will be generated\n",
                          i, image->comps[i].bpp, tmp_str);
            return OPJ_FALSE;
        }
    }

    /* Image size */
    switch (rsiz) {
    case OPJ_PROFILE_CINEMA_2K:
        if (((image->comps[0].w > 2048) | (image->comps[0].h > 1080))) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "JPEG 2000 Profile-3 (2k dc profile) requires:\n"
                          "width <= 2048 and height <= 1080\n"
                          "-> Input image size %d x %d is not compliant\n"
                          "-> Non-profile-3 codestream will be generated\n",
                          image->comps[0].w, image->comps[0].h);
            return OPJ_FALSE;
        }
        break;
    case OPJ_PROFILE_CINEMA_4K:
        if (((image->comps[0].w > 4096) | (image->comps[0].h > 2160))) {
            opj_event_msg(p_manager, EVT_WARNING,
                          "JPEG 2000 Profile-4 (4k dc profile) requires:\n"
                          "width <= 4096 and height <= 2160\n"
                          "-> Image size %d x %d is not compliant\n"
                          "-> Non-profile-4 codestream will be generated\n",
                          image->comps[0].w, image->comps[0].h);
            return OPJ_FALSE;
        }
        break;
    default :
        break;
    }

    return OPJ_TRUE;
}
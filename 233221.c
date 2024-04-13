int imagetopng(opj_image_t * image, const char *write_idf)
{
    FILE * volatile writer = NULL;
    png_structp png = NULL;
    png_infop info = NULL;
    png_bytep volatile row_buf = NULL;
    int nr_comp, color_type;
    volatile int prec;
    png_color_8 sig_bit;
    OPJ_INT32 const* planes[4];
    int i;
    OPJ_INT32* volatile buffer32s = NULL;

    volatile int fails = 1;

    memset(&sig_bit, 0, sizeof(sig_bit));
    prec = (int)image->comps[0].prec;
    planes[0] = image->comps[0].data;
    if (planes[0] == NULL) {
        fprintf(stderr,
                "imagetopng: planes[%d] == NULL.\n", 0);
        fprintf(stderr, "\tAborting\n");
        return 1;
    }
    nr_comp = (int)image->numcomps;

    if (nr_comp > 4) {
        nr_comp = 4;
    }
    for (i = 1; i < nr_comp; ++i) {
        if (image->comps[0].dx != image->comps[i].dx) {
            break;
        }
        if (image->comps[0].dy != image->comps[i].dy) {
            break;
        }
        if (image->comps[0].prec != image->comps[i].prec) {
            break;
        }
        if (image->comps[0].sgnd != image->comps[i].sgnd) {
            break;
        }
        planes[i] = image->comps[i].data;
        if (planes[i] == NULL) {
            fprintf(stderr,
                    "imagetopng: planes[%d] == NULL.\n", i);
            fprintf(stderr, "\tAborting\n");
            return 1;
        }
    }
    if (i != nr_comp) {
        fprintf(stderr,
                "imagetopng: All components shall have the same subsampling, same bit depth, same sign.\n");
        fprintf(stderr, "\tAborting\n");
        return 1;
    }
    for (i = 0; i < nr_comp; ++i) {
        clip_component(&(image->comps[i]), image->comps[0].prec);
    }
    if (prec > 8 && prec < 16) {
        for (i = 0; i < nr_comp; ++i) {
            scale_component(&(image->comps[i]), 16);
        }
        prec = 16;
    } else if (prec < 8 && nr_comp > 1) { /* GRAY_ALPHA, RGB, RGB_ALPHA */
        for (i = 0; i < nr_comp; ++i) {
            scale_component(&(image->comps[i]), 8);
        }
        prec = 8;
    } else if ((prec > 1) && (prec < 8) && ((prec == 6) ||
                                            ((prec & 1) == 1))) { /* GRAY with non native precision */
        if ((prec == 5) || (prec == 6)) {
            prec = 8;
        } else {
            prec++;
        }
        for (i = 0; i < nr_comp; ++i) {
            scale_component(&(image->comps[i]), (OPJ_UINT32)prec);
        }
    }

    if (prec != 1 && prec != 2 && prec != 4 && prec != 8 && prec != 16) {
        fprintf(stderr, "imagetopng: can not create %s\n\twrong bit_depth %d\n",
                write_idf, prec);
        return fails;
    }

    writer = fopen(write_idf, "wb");

    if (writer == NULL) {
        return fails;
    }

    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also check that
     * the library version is compatible with the one used at compile time,
     * in case we are using dynamically linked libraries.  REQUIRED.
     */
    png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                  NULL, NULL, NULL);
    /*png_voidp user_error_ptr, user_error_fn, user_warning_fn); */

    if (png == NULL) {
        goto fin;
    }

    /* Allocate/initialize the image information data.  REQUIRED
     */
    info = png_create_info_struct(png);

    if (info == NULL) {
        goto fin;
    }

    /* Set error handling.  REQUIRED if you are not supplying your own
     * error handling functions in the png_create_write_struct() call.
     */
    if (setjmp(png_jmpbuf(png))) {
        goto fin;
    }

    /* I/O initialization functions is REQUIRED
     */
    png_init_io(png, writer);

    /* Set the image information here.  Width and height are up to 2^31,
     * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
     * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
     * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
     * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
     * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
     * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE.
     * REQUIRED
     *
     * ERRORS:
     *
     * color_type == PNG_COLOR_TYPE_PALETTE && bit_depth > 8
     * color_type == PNG_COLOR_TYPE_RGB && bit_depth < 8
     * color_type == PNG_COLOR_TYPE_GRAY_ALPHA && bit_depth < 8
     * color_type == PNG_COLOR_TYPE_RGB_ALPHA) && bit_depth < 8
     *
     */
    png_set_compression_level(png, Z_BEST_COMPRESSION);

    if (nr_comp >= 3) { /* RGB(A) */
        color_type = PNG_COLOR_TYPE_RGB;
        sig_bit.red = sig_bit.green = sig_bit.blue = (png_byte)prec;
    } else { /* GRAY(A) */
        color_type = PNG_COLOR_TYPE_GRAY;
        sig_bit.gray = (png_byte)prec;
    }
    if ((nr_comp & 1) == 0) { /* ALPHA */
        color_type |= PNG_COLOR_MASK_ALPHA;
        sig_bit.alpha = (png_byte)prec;
    }

    png_set_IHDR(png, info, image->comps[0].w, image->comps[0].h, prec, color_type,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,  PNG_FILTER_TYPE_BASE);

    png_set_sBIT(png, info, &sig_bit);
    /* png_set_gamma(png, 2.2, 1./2.2); */
    /* png_set_sRGB(png, info, PNG_sRGB_INTENT_PERCEPTUAL); */
    png_write_info(png, info);

    /* setup conversion */
    {
        OPJ_SIZE_T rowStride;
        png_size_t png_row_size;

        png_row_size = png_get_rowbytes(png, info);
        rowStride = ((OPJ_SIZE_T)image->comps[0].w * (OPJ_SIZE_T)nr_comp *
                     (OPJ_SIZE_T)prec + 7U) / 8U;
        if (rowStride != (OPJ_SIZE_T)png_row_size) {
            fprintf(stderr, "Invalid PNG row size\n");
            goto fin;
        }
        row_buf = (png_bytep)malloc(png_row_size);
        if (row_buf == NULL) {
            fprintf(stderr, "Can't allocate memory for PNG row\n");
            goto fin;
        }
        buffer32s = (OPJ_INT32*)malloc((OPJ_SIZE_T)image->comps[0].w *
                                       (OPJ_SIZE_T)nr_comp * sizeof(OPJ_INT32));
        if (buffer32s == NULL) {
            fprintf(stderr, "Can't allocate memory for interleaved 32s row\n");
            goto fin;
        }
    }

    /* convert */
    {
        OPJ_SIZE_T width = image->comps[0].w;
        OPJ_UINT32 y;
        convert_32s_PXCX cvtPxToCx = convert_32s_PXCX_LUT[nr_comp];
        convert_32sXXx_C1R cvt32sToPack = NULL;
        OPJ_INT32 adjust = image->comps[0].sgnd ? 1 << (prec - 1) : 0;
        png_bytep row_buf_cpy = row_buf;
        OPJ_INT32* buffer32s_cpy = buffer32s;

        switch (prec) {
        case 1:
        case 2:
        case 4:
        case 8:
            cvt32sToPack = convert_32sXXu_C1R_LUT[prec];
            break;
        case 16:
            cvt32sToPack = convert_32s16u_C1R;
            break;
        default:
            /* never here */
            break;
        }

        for (y = 0; y < image->comps[0].h; ++y) {
            cvtPxToCx(planes, buffer32s_cpy, width, adjust);
            cvt32sToPack(buffer32s_cpy, row_buf_cpy, width * (OPJ_SIZE_T)nr_comp);
            png_write_row(png, row_buf_cpy);
            planes[0] += width;
            planes[1] += width;
            planes[2] += width;
            planes[3] += width;
        }
    }

    png_write_end(png, info);

    fails = 0;

fin:
    if (png) {
        png_destroy_write_struct(&png, &info);
    }
    if (row_buf) {
        free(row_buf);
    }
    if (buffer32s) {
        free(buffer32s);
    }
    fclose(writer);

    if (fails) {
        (void)remove(write_idf);    /* ignore return value */
    }

    return fails;
}/* imagetopng() */
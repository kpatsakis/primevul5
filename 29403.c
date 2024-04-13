zgetbitsrect(i_ctx_t *i_ctx_p)
{	/*
         * alpha? is 0 for no alpha, -1 for alpha first, 1 for alpha last.
         * std_depth is null for native pixels, depth/component for
         * standard color space.
         */
    os_ptr op = osp;
    gx_device *dev;
    gs_int_rect rect;
    gs_get_bits_params_t params;
    int w, h;
    gs_get_bits_options_t options =
        GB_ALIGN_ANY | GB_RETURN_COPY | GB_OFFSET_0 | GB_RASTER_STANDARD |
        GB_PACKING_CHUNKY;
    int depth;
    uint raster;
    int num_rows;
    int code;

    check_read_type(op[-7], t_device);
    dev = op[-7].value.pdevice;
    if (dev == NULL)
        /* This can happen if we invalidated devices on the stack by calling nulldevice after they were pushed */
        return_error(gs_error_undefined);

    check_int_leu(op[-6], dev->width);
    rect.p.x = op[-6].value.intval;
    check_int_leu(op[-5], dev->height);
    rect.p.y = op[-5].value.intval;
    check_int_leu(op[-4], dev->width);
    w = op[-4].value.intval;
    check_int_leu(op[-3], dev->height);
    h = op[-3].value.intval;
    check_type(op[-2], t_integer);
    /*
     * We use if/else rather than switch because the value is long,
     * which is not supported as a switch value in pre-ANSI C.
     */
    if (op[-2].value.intval == -1)
        options |= GB_ALPHA_FIRST;
    else if (op[-2].value.intval == 0)
        options |= GB_ALPHA_NONE;
    else if (op[-2].value.intval == 1)
        options |= GB_ALPHA_LAST;
    else
        return_error(gs_error_rangecheck);
    if (r_has_type(op - 1, t_null)) {
        options |= GB_COLORS_NATIVE;
        depth = dev->color_info.depth;
    } else {
        static const gs_get_bits_options_t depths[17] = {
            0, GB_DEPTH_1, GB_DEPTH_2, 0, GB_DEPTH_4, 0, 0, 0, GB_DEPTH_8,
            0, 0, 0, GB_DEPTH_12, 0, 0, 0, GB_DEPTH_16
        };
        gs_get_bits_options_t depth_option;
        int std_depth;

        check_int_leu(op[-1], 16);
        std_depth = (int)op[-1].value.intval;
        depth_option = depths[std_depth];
        if (depth_option == 0)
            return_error(gs_error_rangecheck);
        options |= depth_option | GB_COLORS_NATIVE;
        depth = (dev->color_info.num_components +
                 (options & GB_ALPHA_NONE ? 0 : 1)) * std_depth;
    }
    if (w == 0)
        return_error(gs_error_rangecheck);
    raster = (w * depth + 7) >> 3;
    check_write_type(*op, t_string);
    num_rows = r_size(op) / raster;
    h = min(h, num_rows);
    if (h == 0)
        return_error(gs_error_rangecheck);
    rect.q.x = rect.p.x + w;
    rect.q.y = rect.p.y + h;
    params.options = options;
    params.data[0] = op->value.bytes;
    code = (*dev_proc(dev, get_bits_rectangle))(dev, &rect, &params, NULL);
    if (code < 0)
        return code;
    make_int(op - 7, h);
    op[-6] = *op;
    r_set_size(op - 6, h * raster);
    pop(6);
    return 0;
}

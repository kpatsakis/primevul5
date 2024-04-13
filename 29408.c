zmakewordimagedevice(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    os_ptr op1 = op - 1;
    gs_matrix imat;
    gx_device *new_dev;
    const byte *colors;
    int colors_size;
    int code;

    check_int_leu(op[-3], max_uint >> 1);	/* width */
    check_int_leu(op[-2], max_uint >> 1);	/* height */
    check_type(*op, t_boolean);
    if (r_has_type(op1, t_null)) {	/* true color */
        colors = 0;
        colors_size = -24;	/* 24-bit true color */
    } else if (r_has_type(op1, t_integer)) {
        /*
         * We use if/else rather than switch because the value is long,
         * which is not supported as a switch value in pre-ANSI C.
         */
        if (op1->value.intval != 16 && op1->value.intval != 24 &&
            op1->value.intval != 32
            )
            return_error(gs_error_rangecheck);
        colors = 0;
        colors_size = -op1->value.intval;
    } else {
        check_type(*op1, t_string);	/* palette */
        if (r_size(op1) > 3 * 256)
            return_error(gs_error_rangecheck);
        colors = op1->value.bytes;
        colors_size = r_size(op1);
    }
    if ((code = read_matrix(imemory, op - 4, &imat)) < 0)
        return code;
    /* Everything OK, create device */
    code = gs_makewordimagedevice(&new_dev, &imat,
                                  (int)op[-3].value.intval,
                                  (int)op[-2].value.intval,
                                  colors, colors_size,
                                  op->value.boolval, true, imemory);
    if (code == 0) {
        new_dev->memory = imemory;
        make_tav(op - 4, t_device, imemory_space(iimemory) | a_all,
                 pdevice, new_dev);
        pop(4);
    }
    return code;
}

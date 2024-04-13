zcopydevice2(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    gx_device *new_dev;
    int code;

    check_read_type(op[-1], t_device);
    check_type(*op, t_boolean);
    if (op[-1].value.pdevice == NULL)
        /* This can happen if we invalidated devices on the stack by calling nulldevice after they were pushed */
        return_error(gs_error_undefined);

    code = gs_copydevice2(&new_dev, op[-1].value.pdevice, op->value.boolval,
                          imemory);
    if (code < 0)
        return code;
    new_dev->memory = imemory;
    make_tav(op - 1, t_device, icurrent_space | a_all, pdevice, new_dev);
    pop(1);
    return 0;
}
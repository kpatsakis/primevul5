zget_device_params(i_ctx_t *i_ctx_p, bool is_hardware)
{
    os_ptr op = osp;
    ref rkeys;
    gx_device *dev;
    stack_param_list list;
    int code;
    ref *pmark;

    check_read_type(op[-1], t_device);

    if(!r_has_type(op, t_null)) {
        check_type(*op, t_dictionary);
    }
    rkeys = *op;
    dev = op[-1].value.pdevice;
    if (op[-1].value.pdevice == NULL)
        /* This can happen if we invalidated devices on the stack by calling nulldevice after they were pushed */
        return_error(gs_error_undefined);
    pop(1);
    stack_param_list_write(&list, &o_stack, &rkeys, iimemory);
    code = gs_get_device_or_hardware_params(dev, (gs_param_list *) & list,
                                            is_hardware);
    if (code < 0) {
        /* We have to put back the top argument. */
        if (list.count > 0)
            ref_stack_pop(&o_stack, list.count * 2 - 1);
        else
            ref_stack_push(&o_stack, 1);
        *osp = rkeys;
        return code;
    }
    pmark = ref_stack_index(&o_stack, list.count * 2);
    make_mark(pmark);
    return 0;
}

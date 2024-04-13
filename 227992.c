zdevicename(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    const char *dname;

    check_read_type(*op, t_device);
    if (op->value.pdevice == NULL)
        /* This can happen if we invalidated devices on the stack by calling nulldevice after they were pushed */
        return_error(gs_error_undefined);

    dname = op->value.pdevice->dname;
    make_const_string(op, avm_foreign | a_readonly, strlen(dname),
                      (const byte *)dname);
    return 0;
}
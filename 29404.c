zgetdefaultdevice(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    const gx_device *dev;

    dev = gs_getdefaultlibdevice(imemory);
    if (dev == 0) /* couldn't find a default device */
        return_error(gs_error_unknownerror);
    push(1);
    make_tav(op, t_device, avm_foreign | a_readonly, pdevice,
                (gx_device *) dev);
    return 0;
}

zcurrentoutputdevice(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    gx_device *odev = NULL, *dev = gs_currentdevice(igs);
    gs_ref_memory_t *mem = (gs_ref_memory_t *) dev->memory;
    int code = dev_proc(dev, dev_spec_op)(dev,
                        gxdso_current_output_device, (void *)&odev, 0);
    if (code < 0)
        return code;

    push(1);
    make_tav(op, t_device,
             (mem == 0 ? avm_foreign : imemory_space(mem)) | a_all,
             pdevice, odev);
    return 0;
}

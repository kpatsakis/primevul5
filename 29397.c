zcurrentdevice(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    gx_device *dev = gs_currentdevice(igs);
    gs_ref_memory_t *mem = (gs_ref_memory_t *) dev->memory;

    push(1);
    make_tav(op, t_device,
             (mem == 0 ? avm_foreign : imemory_space(mem)) | a_all,
             pdevice, dev);
    return 0;
}

znulldevice(i_ctx_t *i_ctx_p)
{
    int code = gs_nulldevice(igs);
    invalidate_stack_devices(i_ctx_p);
    clear_pagedevice(istate);
    return code;
}

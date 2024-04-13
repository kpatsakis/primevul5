interp_reclaim(i_ctx_t **pi_ctx_p, int space)
{
    i_ctx_t *i_ctx_p = *pi_ctx_p;
    gs_gc_root_t ctx_root;
    int code;

#ifdef DEBUG
    if (gs_debug_c(gs_debug_flag_gc_disable))
        return 0;
#endif

    gs_register_struct_root(imemory_system, &ctx_root,
                            (void **)pi_ctx_p, "interp_reclaim(pi_ctx_p)");
    code = (*idmemory->reclaim)(idmemory, space);
    i_ctx_p = *pi_ctx_p;        /* may have moved */
    gs_unregister_root(imemory_system, &ctx_root, "interp_reclaim(pi_ctx_p)");
    return code;
}
do_cmdkey_command(int key UNUSED, int flags)
{
    int	    res;
#ifdef FEAT_EVAL
    sctx_T  save_current_sctx = {-1, 0, 0, 0};

    if (key == K_SCRIPT_COMMAND && last_used_map != NULL)
    {
	save_current_sctx = current_sctx;
	current_sctx = last_used_map->m_script_ctx;
    }
#endif

    res = do_cmdline(NULL, getcmdkeycmd, NULL, flags);

#ifdef FEAT_EVAL
    if (save_current_sctx.sc_sid >= 0)
	current_sctx = save_current_sctx;
#endif

    return res;
}
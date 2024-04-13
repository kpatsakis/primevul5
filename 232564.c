block_autocmds(void)
{
# ifdef FEAT_EVAL
    /* Remember the value of v:termresponse. */
    if (autocmd_blocked == 0)
	old_termresponse = get_vim_var_str(VV_TERMRESPONSE);
# endif
    ++autocmd_blocked;
}
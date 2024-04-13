apply_autocmds_retval(
    event_T	event,
    char_u	*fname,	    /* NULL or empty means use actual file name */
    char_u	*fname_io,  /* fname to use for <afile> on cmdline */
    int		force,	    /* when TRUE, ignore autocmd_busy */
    buf_T	*buf,	    /* buffer for <abuf> */
    int		*retval)    /* pointer to caller's retval */
{
    int		did_cmd;

#ifdef FEAT_EVAL
    if (should_abort(*retval))
	return FALSE;
#endif

    did_cmd = apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, NULL);
    if (did_cmd
#ifdef FEAT_EVAL
	    && aborting()
#endif
	    )
	*retval = FAIL;
    return did_cmd;
}
apply_autocmds(
    event_T	event,
    char_u	*fname,	    /* NULL or empty means use actual file name */
    char_u	*fname_io,  /* fname to use for <afile> on cmdline */
    int		force,	    /* when TRUE, ignore autocmd_busy */
    buf_T	*buf)	    /* buffer for <abuf> */
{
    return apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, NULL);
}
write_lnum_adjust(linenr_T offset)
{
    if (curbuf->b_no_eol_lnum != 0)	/* only if there is a missing eol */
	curbuf->b_no_eol_lnum += offset;
}
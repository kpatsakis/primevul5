check_marks_read(void)
{
    if (!curbuf->b_marks_read && get_viminfo_parameter('\'') > 0
						  && curbuf->b_ffname != NULL)
	read_viminfo(NULL, VIF_WANT_MARKS);

    /* Always set b_marks_read; needed when 'viminfo' is changed to include
     * the ' parameter after opening a buffer. */
    curbuf->b_marks_read = TRUE;
}
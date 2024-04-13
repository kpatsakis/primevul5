msg_add_fname(buf_T *buf, char_u *fname)
{
    if (fname == NULL)
	fname = (char_u *)"-stdin-";
    home_replace(buf, fname, IObuff + 1, IOSIZE - 4, TRUE);
    IObuff[0] = '"';
    STRCAT(IObuff, "\" ");
}
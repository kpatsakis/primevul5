aucmd_prepbuf(
    aco_save_T	*aco,		/* structure to save values in */
    buf_T	*buf)		/* new curbuf */
{
    aco->save_curbuf = curbuf;
    --curbuf->b_nwindows;
    curbuf = buf;
    curwin->w_buffer = buf;
    ++curbuf->b_nwindows;
}
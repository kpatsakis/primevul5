int RECORD_LAYER_write_pending(const RECORD_LAYER *rl)
{
    return (rl->numwpipes > 0)
        && SSL3_BUFFER_get_left(&rl->wbuf[rl->numwpipes - 1]) != 0;
}
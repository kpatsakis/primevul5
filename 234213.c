int RECORD_LAYER_read_pending(const RECORD_LAYER *rl)
{
    return SSL3_BUFFER_get_left(&rl->rbuf) != 0;
}
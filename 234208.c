void RECORD_LAYER_release(RECORD_LAYER *rl)
{
    if (SSL3_BUFFER_is_initialised(&rl->rbuf))
        ssl3_release_read_buffer(rl->s);
    if (rl->numwpipes > 0)
        ssl3_release_write_buffer(rl->s);
    SSL3_RECORD_release(rl->rrec, SSL_MAX_PIPELINES);
}
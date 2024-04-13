int RECORD_LAYER_set_data(RECORD_LAYER *rl, const unsigned char *buf, int len)
{
    rl->packet_length = len;
    if (len != 0) {
        rl->rstate = SSL_ST_READ_HEADER;
        if (!SSL3_BUFFER_is_initialised(&rl->rbuf))
            if (!ssl3_setup_read_buffer(rl->s))
                return 0;
    }

    rl->packet = SSL3_BUFFER_get_buf(&rl->rbuf);
    SSL3_BUFFER_set_data(&rl->rbuf, buf, len);

    return 1;
}
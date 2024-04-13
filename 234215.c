void SSL_set_default_read_buffer_len(SSL *s, size_t len)
{
    SSL3_BUFFER_set_default_len(RECORD_LAYER_get_rbuf(&s->rlayer), len);
}
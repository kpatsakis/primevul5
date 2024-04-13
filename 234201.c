void SSL_CTX_set_default_read_buffer_len(SSL_CTX *ctx, size_t len)
{
    ctx->default_read_buf_len = len;
}
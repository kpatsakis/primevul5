int ssh_buffer_allocate_size(struct ssh_buffer_struct *buffer,
                             uint32_t len)
{
    buffer_verify(buffer);

    if (buffer->allocated < len) {
        if (buffer->pos > 0) {
            buffer_shift(buffer);
        }
        if (realloc_buffer(buffer, len) < 0) {
            return -1;
        }
    }

    buffer_verify(buffer);

    return 0;
}
static void buffer_shift(ssh_buffer buffer)
{
    size_t burn_pos = buffer->pos;

    buffer_verify(buffer);

    if (buffer->pos == 0) {
        return;
    }
    memmove(buffer->data,
            buffer->data + buffer->pos,
            buffer->used - buffer->pos);
    buffer->used -= buffer->pos;
    buffer->pos = 0;

    if (buffer->secure) {
        void *ptr = buffer->data + buffer->used;
        explicit_bzero(ptr, burn_pos);
    }

    buffer_verify(buffer);
}
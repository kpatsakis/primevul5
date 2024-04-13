void ssh_buffer_free(struct ssh_buffer_struct *buffer)
{
    if (buffer == NULL) {
        return;
    }
    buffer_verify(buffer);

    if (buffer->secure && buffer->allocated > 0) {
        /* burn the data */
        explicit_bzero(buffer->data, buffer->allocated);
        SAFE_FREE(buffer->data);

        explicit_bzero(buffer, sizeof(struct ssh_buffer_struct));
    } else {
        SAFE_FREE(buffer->data);
    }
    SAFE_FREE(buffer);
}
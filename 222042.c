int ssh_buffer_validate_length(struct ssh_buffer_struct *buffer, size_t len)
{
    if (buffer->pos + len < len || buffer->pos + len > buffer->used) {
        return SSH_ERROR;
    }

    return SSH_OK;
}
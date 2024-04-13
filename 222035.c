uint32_t ssh_buffer_get_data(struct ssh_buffer_struct *buffer, void *data, uint32_t len)
{
    int rc;

    /*
     * Check for a integer overflow first, then check if not enough data is in
     * the buffer.
     */
    rc = ssh_buffer_validate_length(buffer, len);
    if (rc != SSH_OK) {
        return 0;
    }
    memcpy(data,buffer->data+buffer->pos,len);
    buffer->pos+=len;
    return len;   /* no yet support for partial reads (is it really needed ?? ) */
}
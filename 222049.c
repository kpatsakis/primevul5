struct ssh_buffer_struct *ssh_buffer_new(void)
{
    struct ssh_buffer_struct *buf = NULL;
    int rc;

    buf = calloc(1, sizeof(struct ssh_buffer_struct));
    if (buf == NULL) {
        return NULL;
    }

    /*
     * Always preallocate 64 bytes.
     *
     * -1 for ralloc_buffer magic.
     */
    rc = ssh_buffer_allocate_size(buf, 64 - 1);
    if (rc != 0) {
        SAFE_FREE(buf);
        return NULL;
    }
    buffer_verify(buf);

    return buf;
}
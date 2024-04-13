int ssh_buffer_add_buffer(struct ssh_buffer_struct *buffer,
    struct ssh_buffer_struct *source)
{
    int rc;

    rc = ssh_buffer_add_data(buffer,
                             ssh_buffer_get(source),
                             ssh_buffer_get_len(source));
    if (rc < 0) {
        return -1;
    }

    return 0;
}
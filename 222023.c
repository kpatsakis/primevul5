int ssh_buffer_add_u16(struct ssh_buffer_struct *buffer,uint16_t data)
{
    int rc;

    rc = ssh_buffer_add_data(buffer, &data, sizeof(data));
    if (rc < 0) {
        return -1;
    }

    return 0;
}
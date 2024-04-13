int ssh_buffer_add_u8(struct ssh_buffer_struct *buffer,uint8_t data)
{
    int rc;

    rc = ssh_buffer_add_data(buffer, &data, sizeof(uint8_t));
    if (rc < 0) {
        return -1;
    }

    return 0;
}
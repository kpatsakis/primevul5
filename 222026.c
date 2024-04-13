int ssh_buffer_get_u8(struct ssh_buffer_struct *buffer, uint8_t *data){
    return ssh_buffer_get_data(buffer,data,sizeof(uint8_t));
}
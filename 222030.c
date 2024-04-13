int ssh_buffer_get_u32(struct ssh_buffer_struct *buffer, uint32_t *data){
    return ssh_buffer_get_data(buffer,data,sizeof(uint32_t));
}
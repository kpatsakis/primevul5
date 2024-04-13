int ssh_buffer_get_u64(struct ssh_buffer_struct *buffer, uint64_t *data){
    return ssh_buffer_get_data(buffer,data,sizeof(uint64_t));
}
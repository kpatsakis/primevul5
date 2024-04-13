ssh_buffer_get_ssh_string(struct ssh_buffer_struct *buffer)
{
    uint32_t stringlen;
    uint32_t hostlen;
    struct ssh_string_struct *str = NULL;
    int rc;

    rc = ssh_buffer_get_u32(buffer, &stringlen);
    if (rc == 0) {
        return NULL;
    }
    hostlen = ntohl(stringlen);
    /* verify if there is enough space in buffer to get it */
    rc = ssh_buffer_validate_length(buffer, hostlen);
    if (rc != SSH_OK) {
      return NULL; /* it is indeed */
    }
    str = ssh_string_new(hostlen);
    if (str == NULL) {
        return NULL;
    }

    stringlen = ssh_buffer_get_data(buffer, ssh_string_data(str), hostlen);
    if (stringlen != hostlen) {
        /* should never happen */
        SAFE_FREE(str);
        return NULL;
    }

    return str;
}
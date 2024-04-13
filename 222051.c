int _ssh_buffer_pack(struct ssh_buffer_struct *buffer,
                     const char *format,
                     size_t argc,
                     ...)
{
    va_list ap;
    int rc;

    if (argc > 256) {
        return SSH_ERROR;
    }

    va_start(ap, argc);
    rc = ssh_buffer_pack_allocate_va(buffer, format, argc, ap);
    va_end(ap);

    if (rc != SSH_OK) {
        return rc;
    }

    va_start(ap, argc);
    rc = ssh_buffer_pack_va(buffer, format, argc, ap);
    va_end(ap);

    return rc;
}
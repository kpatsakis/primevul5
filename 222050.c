int _ssh_buffer_unpack(struct ssh_buffer_struct *buffer,
                       const char *format,
                       size_t argc,
                       ...)
{
    va_list ap;
    int rc;

    va_start(ap, argc);
    rc = ssh_buffer_unpack_va(buffer, format, argc, ap);
    va_end(ap);
    return rc;
}
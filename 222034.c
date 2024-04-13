static void buffer_verify(ssh_buffer buf)
{
    bool do_abort = false;

    if (buf->data == NULL) {
        return;
    }

    if (buf->used > buf->allocated) {
        fprintf(stderr,
                "BUFFER ERROR: allocated %zu, used %zu\n",
                buf->allocated,
                buf->used);
        do_abort = true;
    }
    if (buf->pos > buf->used) {
        fprintf(stderr,
                "BUFFER ERROR: position %zu, used %zu\n",
                buf->pos,
                buf->used);
        do_abort = true;
    }
    if (buf->pos > buf->allocated) {
        fprintf(stderr,
                "BUFFER ERROR: position %zu, allocated %zu\n",
                buf->pos,
                buf->allocated);
        do_abort = true;
    }
    if (do_abort) {
        abort();
    }
}
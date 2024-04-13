lyxml_print_fd(int fd, const struct lyxml_elem *elem, int options)
{
    FUN_IN;

    struct lyout out;

    if (fd < 0 || !elem) {
        return 0;
    }

    memset(&out, 0, sizeof out);

    out.type = LYOUT_FD;
    out.method.fd = fd;

    if (options & LYXML_PRINT_SIBLINGS) {
        return dump_siblings(&out, elem, options);
    } else {
        return dump_elem(&out, elem, 0, options, 1);
    }
}
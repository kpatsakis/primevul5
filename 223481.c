lyxml_print_mem(char **strp, const struct lyxml_elem *elem, int options)
{
    FUN_IN;

    struct lyout out;
    int r;

    if (!strp || !elem) {
        return 0;
    }

    memset(&out, 0, sizeof out);

    out.type = LYOUT_MEMORY;

    if (options & LYXML_PRINT_SIBLINGS) {
        r = dump_siblings(&out, elem, options);
    } else {
        r = dump_elem(&out, elem, 0, options, 1);
    }

    *strp = out.method.mem.buf;
    return r;
}
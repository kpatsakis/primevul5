lyxml_print_clb(ssize_t (*writeclb)(void *arg, const void *buf, size_t count), void *arg, const struct lyxml_elem *elem, int options)
{
    FUN_IN;

    struct lyout out;

    if (!writeclb || !elem) {
        return 0;
    }

    memset(&out, 0, sizeof out);

    out.type = LYOUT_CALLBACK;
    out.method.clb.f = writeclb;
    out.method.clb.arg = arg;

    if (options & LYXML_PRINT_SIBLINGS) {
        return dump_siblings(&out, elem, options);
    } else {
        return dump_elem(&out, elem, 0, options, 1);
    }
}
EXPORTED char *dumpentryatt(const struct entryattlist *l)
{
    struct buf buf = BUF_INITIALIZER;

    const struct entryattlist *ee;
    buf_printf(&buf, "(");
    const char *sp = "";
    const struct attvaluelist *av;
    for (ee = l ; ee ; ee = ee->next) {
        buf_printf(&buf, "%s%s (", sp, ee->entry);
        const char *insp = "";
        for (av = ee->attvalues ; av ; av = av->next) {
            buf_printf(&buf, "%s%s %s", insp, av->attrib, buf_cstring(&av->value));
            insp = " ";
        }
        buf_printf(&buf, ")");
        sp = " ";
    }
    buf_printf(&buf, ")");

    char *res = buf_release(&buf);
    buf_free(&buf);

    return res;
}
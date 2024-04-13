QemuOpts *vnc_parse(const char *str, Error **errp)
{
    QemuOptsList *olist = qemu_find_opts("vnc");
    QemuOpts *opts = qemu_opts_parse(olist, str, true, errp);
    const char *id;

    if (!opts) {
        return NULL;
    }

    id = qemu_opts_id(opts);
    if (!id) {
        /* auto-assign id if not present */
        vnc_auto_assign_id(olist, opts);
    }
    return opts;
}
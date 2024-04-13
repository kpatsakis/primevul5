static void vnc_auto_assign_id(QemuOptsList *olist, QemuOpts *opts)
{
    int i = 2;
    char *id;

    id = g_strdup("default");
    while (qemu_opts_find(olist, id)) {
        g_free(id);
        id = g_strdup_printf("vnc%d", i++);
    }
    qemu_opts_set_id(opts, id);
}
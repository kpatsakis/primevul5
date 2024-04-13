static int failover_set_primary(void *opaque, QemuOpts *opts, Error **errp)
{
    FailoverId *fid = opaque;
    const char *standby_id = qemu_opt_get(opts, "failover_pair_id");

    if (g_strcmp0(standby_id, fid->n->netclient_name) == 0) {
        fid->id = g_strdup(opts->id);
        return 1;
    }

    return 0;
}
static void failover_add_primary(VirtIONet *n, Error **errp)
{
    Error *err = NULL;
    QemuOpts *opts;
    char *id;
    DeviceState *dev = failover_find_primary_device(n);

    if (dev) {
        return;
    }

    id = failover_find_primary_device_id(n);
    if (!id) {
        error_setg(errp, "Primary device not found");
        error_append_hint(errp, "Virtio-net failover will not work. Make "
                          "sure primary device has parameter"
                          " failover_pair_id=%s\n", n->netclient_name);
        return;
    }
    opts = qemu_opts_find(qemu_find_opts("device"), id);
    g_assert(opts); /* cannot be NULL because id was found using opts list */
    dev = qdev_device_add(opts, &err);
    if (err) {
        qemu_opts_del(opts);
    } else {
        object_unref(OBJECT(dev));
    }
    error_propagate(errp, err);
}
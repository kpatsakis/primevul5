static char *failover_find_primary_device_id(VirtIONet *n)
{
    Error *err = NULL;
    FailoverId fid;

    fid.n = n;
    if (!qemu_opts_foreach(qemu_find_opts("device"),
                           failover_set_primary, &fid, &err)) {
        return NULL;
    }
    return fid.id;
}
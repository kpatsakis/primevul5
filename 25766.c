static DeviceState *failover_find_primary_device(VirtIONet *n)
{
    char *id = failover_find_primary_device_id(n);

    if (!id) {
        return NULL;
    }

    return qdev_find_recursive(sysbus_get_default(), id);
}
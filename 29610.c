SPICE_GNUC_VISIBLE void spice_server_vm_start(SpiceServer *s)
{
    RingItem *item;

    spice_assert(s == reds);
    reds->vm_running = TRUE;
    RING_FOREACH(item, &reds->char_devs_states) {
        SpiceCharDeviceStateItem *st_item;

        st_item = SPICE_CONTAINEROF(item, SpiceCharDeviceStateItem, link);
        spice_char_device_start(st_item->st);
    }
    red_dispatcher_on_vm_start();
}

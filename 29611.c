SPICE_GNUC_VISIBLE void spice_server_vm_stop(SpiceServer *s)
{
    RingItem *item;

    spice_assert(s == reds);
    reds->vm_running = FALSE;
    RING_FOREACH(item, &reds->char_devs_states) {
        SpiceCharDeviceStateItem *st_item;

        st_item = SPICE_CONTAINEROF(item, SpiceCharDeviceStateItem, link);
        spice_char_device_stop(st_item->st);
    }
    red_dispatcher_on_vm_stop();
}

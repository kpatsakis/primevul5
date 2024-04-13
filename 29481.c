static void reds_char_device_remove_state(SpiceCharDeviceState *st)
{
    RingItem *item;

    RING_FOREACH(item, &reds->char_devs_states) {
        SpiceCharDeviceStateItem *st_item;

        st_item = SPICE_CONTAINEROF(item, SpiceCharDeviceStateItem, link);
        if (st_item->st == st) {
            ring_remove(item);
            free(st_item);
            return;
        }
    }
    spice_error("char dev state not found %p", st);
}

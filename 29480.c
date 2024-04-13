static void reds_char_device_add_state(SpiceCharDeviceState *st)
{
    SpiceCharDeviceStateItem *item = spice_new0(SpiceCharDeviceStateItem, 1);

    item->st = st;

    ring_add(&reds->char_devs_states, &item->link);
}

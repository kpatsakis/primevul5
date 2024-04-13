SPICE_GNUC_VISIBLE void spice_server_char_device_wakeup(SpiceCharDeviceInstance* sin)
{
    if (!sin->st) {
        spice_warning("no SpiceCharDeviceState attached to instance %p", sin);
        return;
    }
    spice_char_device_wakeup(sin->st);
}

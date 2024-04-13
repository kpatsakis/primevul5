void reds_on_char_device_state_destroy(SpiceCharDeviceState *dev)
{
    reds_char_device_remove_state(dev);
}

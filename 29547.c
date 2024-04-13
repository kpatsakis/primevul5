void reds_on_main_mouse_mode_request(void *message, size_t size)
{
    switch (((SpiceMsgcMainMouseModeRequest *)message)->mode) {
    case SPICE_MOUSE_MODE_CLIENT:
        if (reds->is_client_mouse_allowed) {
            reds_set_mouse_mode(SPICE_MOUSE_MODE_CLIENT);
        } else {
            spice_info("client mouse is disabled");
        }
        break;
    case SPICE_MOUSE_MODE_SERVER:
        reds_set_mouse_mode(SPICE_MOUSE_MODE_SERVER);
        break;
    default:
        spice_warning("unsupported mouse mode");
    }
}

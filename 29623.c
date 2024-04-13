static void vdi_port_on_free_self_token(void *opaque)
{

    if (inputs_inited() && reds->pending_mouse_event) {
        spice_debug("pending mouse event");
        reds_handle_agent_mouse_event(inputs_get_mouse_state());
    }
}

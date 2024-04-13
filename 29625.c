static int vdi_port_read_buf_process(int port, VDIReadBuf *buf)
{
    VDIPortState *state = &reds->agent_state;
    int res;

    switch (port) {
    case VDP_CLIENT_PORT: {
        res = agent_msg_filter_process_data(&state->read_filter,
                                            buf->data, buf->len);
        switch (res) {
        case AGENT_MSG_FILTER_OK:
            return TRUE;
        case AGENT_MSG_FILTER_DISCARD:
            return FALSE;
        case AGENT_MSG_FILTER_PROTO_ERROR:
            reds_agent_remove();
            return FALSE;
        }
    }
    case VDP_SERVER_PORT:
        return FALSE;
    default:
        spice_warning("invalid port");
        reds_agent_remove();
        return FALSE;
    }
}

static SpiceCharDeviceMsgToClient *vdi_port_read_one_msg_from_device(SpiceCharDeviceInstance *sin,
                                                                     void *opaque)
{
    VDIPortState *state = &reds->agent_state;
    SpiceCharDeviceInterface *sif;
    VDIReadBuf *dispatch_buf;
    int n;

    if (!vdagent) {
        return NULL;
    }
    spice_assert(vdagent == sin);
    sif = SPICE_CONTAINEROF(vdagent->base.sif, SpiceCharDeviceInterface, base);
    while (vdagent) {
        switch (state->read_state) {
        case VDI_PORT_READ_STATE_READ_HEADER:
            n = sif->read(vdagent, state->receive_pos, state->receive_len);
            if (!n) {
                return NULL;
            }
            if ((state->receive_len -= n)) {
                state->receive_pos += n;
                return NULL;
            }
            state->message_receive_len = state->vdi_chunk_header.size;
            state->read_state = VDI_PORT_READ_STATE_GET_BUFF;
        case VDI_PORT_READ_STATE_GET_BUFF: {
            if (!(state->current_read_buf = vdi_port_read_buf_get())) {
                return NULL;
            }
            state->receive_pos = state->current_read_buf->data;
            state->receive_len = MIN(state->message_receive_len,
                                    sizeof(state->current_read_buf->data));
            state->current_read_buf->len = state->receive_len;
            state->message_receive_len -= state->receive_len;
            state->read_state = VDI_PORT_READ_STATE_READ_DATA;
        }
        case VDI_PORT_READ_STATE_READ_DATA:
            n = sif->read(vdagent, state->receive_pos, state->receive_len);
            if (!n) {
                return NULL;
            }
            if ((state->receive_len -= n)) {
                state->receive_pos += n;
                break;
            }
            dispatch_buf = state->current_read_buf;
            state->current_read_buf = NULL;
            state->receive_pos = NULL;
            if (state->message_receive_len == 0) {
                state->read_state = VDI_PORT_READ_STATE_READ_HEADER;
                state->receive_pos = (uint8_t *)&state->vdi_chunk_header;
                state->receive_len = sizeof(state->vdi_chunk_header);
            } else {
                state->read_state = VDI_PORT_READ_STATE_GET_BUFF;
            }
            if (vdi_port_read_buf_process(state->vdi_chunk_header.port, dispatch_buf)) {
                return dispatch_buf;
            } else {
                vdi_port_read_buf_unref(dispatch_buf);
            }
        } /* END switch */
    } /* END while */
    return NULL;
}

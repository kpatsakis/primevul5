void reds_release_agent_data_buffer(uint8_t *buf)
{
    VDIPortState *dev_state = &reds->agent_state;

    if (!dev_state->recv_from_client_buf) {
        free(buf);
        return;
    }

    spice_assert(buf == dev_state->recv_from_client_buf->buf + sizeof(VDIChunkHeader));
    if (!dev_state->recv_from_client_buf_pushed) {
        spice_char_device_write_buffer_release(reds->agent_state.base,
                                               dev_state->recv_from_client_buf);
    }
    dev_state->recv_from_client_buf = NULL;
    dev_state->recv_from_client_buf_pushed = FALSE;
}

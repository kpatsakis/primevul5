int red_channel_client_handle_message(RedChannelClient *rcc, uint32_t size,
                                      uint16_t type, void *message)
{
    switch (type) {
    case SPICE_MSGC_ACK_SYNC:
        if (size != sizeof(uint32_t)) {
            spice_printerr("bad message size");
            return FALSE;
        }
        rcc->ack_data.client_generation = *(uint32_t *)(message);
        break;
    case SPICE_MSGC_ACK:
        if (rcc->ack_data.client_generation == rcc->ack_data.generation) {
            rcc->ack_data.messages_window -= rcc->ack_data.client_window;
            red_channel_client_push(rcc);
        }
        break;
    case SPICE_MSGC_DISCONNECTING:
        break;
    case SPICE_MSGC_MIGRATE_FLUSH_MARK:
        if (!rcc->wait_migrate_flush_mark) {
            spice_error("unexpected flush mark");
            return FALSE;
        }
        red_channel_handle_migrate_flush_mark(rcc);
        rcc->wait_migrate_flush_mark = FALSE;
        break;
    case SPICE_MSGC_MIGRATE_DATA:
        red_channel_handle_migrate_data(rcc, size, message);
        break;
    case SPICE_MSGC_PONG:
        red_channel_client_handle_pong(rcc, message);
        break;
    default:
        spice_printerr("invalid message type %u", type);
        return FALSE;
    }
    return TRUE;
}

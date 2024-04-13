static void red_channel_client_unref(RedChannelClient *rcc)
{
    if (!--rcc->refs) {
        if (rcc->send_data.main.marshaller) {
            spice_marshaller_destroy(rcc->send_data.main.marshaller);
        }

        if (rcc->send_data.urgent.marshaller) {
            spice_marshaller_destroy(rcc->send_data.urgent.marshaller);
        }

        red_channel_client_destroy_remote_caps(rcc);
        if (rcc->channel) {
            red_channel_unref(rcc->channel);
        }
        free(rcc);
    }
}

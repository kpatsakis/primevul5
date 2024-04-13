static void reds_client_monitors_config_cleanup(void)
{
    RedsClientMonitorsConfig *cmc = &reds->client_monitors_config;

    cmc->buffer_size = cmc->buffer_pos = 0;
    free(cmc->buffer);
    cmc->buffer = NULL;
    cmc->mcc = NULL;
}

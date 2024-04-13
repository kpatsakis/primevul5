hmfcb(void *clientp, const char *msg, const char *url)
{
    CbData *data = clientp;
    if (data->hmfcb)
        return data->hmfcb(data->userdata, msg, url, data->metadata);
    return LR_CB_OK;
}
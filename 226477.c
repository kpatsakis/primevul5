static CbData *cbdata_new(void *userdata,
                          void *cbdata,
                          LrProgressCb progresscb,
                          LrHandleMirrorFailureCb hmfcb,
                          const char *metadata)
{
    CbData *data = calloc(1, sizeof(*data));
    data->userdata = userdata;
    data->cbdata = cbdata;
    data->progresscb = progresscb;
    data->hmfcb = hmfcb;
    data->metadata = g_strdup(metadata);
    return data;
}
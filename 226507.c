cbdata_free(CbData *data)
{
    if (!data) return;
    free(data->metadata);
    free(data);
}
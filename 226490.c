lr_get_metadata_failure_callback(const LrHandle *handle)
{
    CbData *cbdata = NULL;
    if (handle->hmfcb) {
        cbdata = cbdata_new(handle->user_data,
                            NULL,
                            NULL,
                            handle->hmfcb,
                            "repomd.xml");
    }
    return cbdata;
}
progresscb(void *clientp, double total_to_download, double downloaded)
{
    CbData *data = clientp;
    if (data->progresscb)
        return data->progresscb(data->userdata, total_to_download, downloaded);
    return LR_CB_OK;
}
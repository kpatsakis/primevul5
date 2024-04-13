virSecuritySELinuxDriverClose(virSecurityManager *mgr)
{
    virSecuritySELinuxData *data = virSecurityManagerGetPrivateData(mgr);

    if (!data)
        return 0;

    if (data->label_handle)
        selabel_close(data->label_handle);

    virHashFree(data->mcs);

    VIR_FREE(data->domain_context);
    VIR_FREE(data->alt_domain_context);
    VIR_FREE(data->file_context);
    VIR_FREE(data->content_context);

    return 0;
}
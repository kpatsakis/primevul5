void wsrep_sst_auth_init (const char* value)
{
    if (wsrep_sst_auth == value) wsrep_sst_auth = NULL;
    if (value) sst_auth_real_set (value);
}
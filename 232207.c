static bool sst_auth_real_set (const char* value)
{
    const char* v = strdup (value);

    if (v)
    {
        if (sst_auth_real) free (const_cast<char*>(sst_auth_real));
        sst_auth_real = v;

        if (strlen(sst_auth_real))
        {
          if (wsrep_sst_auth)
          {
            my_free ((void*)wsrep_sst_auth);
            wsrep_sst_auth = my_strdup(WSREP_SST_AUTH_MASK, MYF(0));
            //strncpy (wsrep_sst_auth, WSREP_SST_AUTH_MASK,
            //     sizeof(wsrep_sst_auth) - 1);
          }
          else
            wsrep_sst_auth = my_strdup (WSREP_SST_AUTH_MASK, MYF(0));
        }
        return 0;
    }

    return 1;
}
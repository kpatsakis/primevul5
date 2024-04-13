void wsrep_sst_auth_free()
{
  if (wsrep_sst_auth) { my_free ((void*)wsrep_sst_auth); }
  if (sst_auth_real) { free (const_cast<char*>(sst_auth_real)); }
  wsrep_sst_auth= NULL;
  sst_auth_real= NULL;
}
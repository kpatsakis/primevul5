static int sst_append_auth_env(wsp::env& env, const char* sst_auth)
{
  int const sst_auth_size= strlen(WSREP_SST_AUTH_ENV) + 1 /* = */
    + (sst_auth ? strlen(sst_auth) : 0) + 1 /* \0 */;

  wsp::string sst_auth_str(sst_auth_size); // for automatic cleanup on return
  if (!sst_auth_str()) return -ENOMEM;

  int ret= snprintf(sst_auth_str(), sst_auth_size, "%s=%s",
                    WSREP_SST_AUTH_ENV, sst_auth ? sst_auth : "");

  if (ret < 0 || ret >= sst_auth_size)
  {
    WSREP_ERROR("sst_append_auth_env(): snprintf() failed: %d", ret);
    return (ret < 0 ? ret : -EMSGSIZE);
  }

  env.append(sst_auth_str());
  return -env.error();
}
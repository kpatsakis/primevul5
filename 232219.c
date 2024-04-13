bool wsrep_sst_auth_update (sys_var *self, THD* thd, enum_var_type type)
{
    return sst_auth_real_set (wsrep_sst_auth);
}
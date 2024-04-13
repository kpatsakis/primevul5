bool wsrep_sst_method_check (sys_var *self, THD* thd, set_var* var)
{
    char   buff[FN_REFLEN];
    String str(buff, sizeof(buff), system_charset_info), *res;
    const char* c_str = NULL;

    if ((res   = var->value->val_str(&str)) &&
        (c_str = res->c_ptr()) &&
        strlen(c_str) > 0)
        return 0;

    my_error(ER_WRONG_VALUE_FOR_VAR, MYF(0), "wsrep_sst_method", c_str ? c_str : "NULL");
    return 1;
}
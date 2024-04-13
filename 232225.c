bool  wsrep_sst_receive_address_check (sys_var *self, THD* thd, set_var* var)
{
    const char* c_str = var->value->str_value.c_ptr();

    if (sst_receive_address_check (c_str))
    {
        my_error(ER_WRONG_VALUE_FOR_VAR, MYF(0), "wsrep_sst_receive_address", c_str ? c_str : "NULL");
        return 1;
    }

    return 0;
}
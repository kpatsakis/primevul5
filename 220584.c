addr_error(cmd_addr_T addr_type)
{
    if (addr_type == ADDR_NONE)
	emsg(_(e_norange));
    else
	emsg(_(e_invalid_range));
}
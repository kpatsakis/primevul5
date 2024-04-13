table_check_use(struct table *t, uint32_t tmask, uint32_t smask)
{
	return table_check_type(t, tmask) && table_check_service(t, smask);
}
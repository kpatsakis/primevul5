table_check_service(struct table *t, uint32_t mask)
{
	return t->t_backend->services & mask;
}
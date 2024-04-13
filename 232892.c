table_check_type(struct table *t, uint32_t mask)
{
	return t->t_type & mask;
}
table_open(struct table *t)
{
	if (t->t_backend->open == NULL)
		return (1);
	return (t->t_backend->open(t));
}
table_update(struct table *t)
{
	if (t->t_backend->update == NULL)
		return (1);
	return (t->t_backend->update(t));
}
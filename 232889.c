table_config(struct table *t)
{
	if (t->t_backend->config == NULL)
		return (1);
	return (t->t_backend->config(t));
}
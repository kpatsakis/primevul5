table_close(struct table *t)
{
	if (t->t_backend->close)
		t->t_backend->close(t);
}
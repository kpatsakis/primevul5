PQfreeCancel(PGcancel *cancel)
{
	if (cancel)
		free(cancel);
}
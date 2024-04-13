void qh_deinit(const char *path)
{
	struct query_handler *qh   = NULL;

	for (qh = qhandlers; qh != NULL; qh = qh->next_qh) {

		qh_deregister_handler(qh->name);
	}

	dkhash_destroy(qh_table);
	qh_table = NULL;
	qhandlers = NULL;

	if (path == NULL) {
		return;
	}

	unlink(path);
}
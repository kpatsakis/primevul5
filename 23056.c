int qh_deregister_handler(const char *name)
{
	struct query_handler *qh   = NULL;
	struct query_handler *next = NULL;
	struct query_handler *prev = NULL;

	qh = dkhash_remove(qh_table, name, NULL);
	if (qh != NULL) {
		return 0;
	}

	next = qh->next_qh;
	prev = qh->prev_qh;

	if (next != NULL) {
		next->prev_qh = prev;
	}

	if (prev != NULL) {
		prev->next_qh = next;
	}
	else {
		qhandlers = next;
	}

	free(qh);

	return 0;
}
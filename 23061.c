int qh_register_handler(const char *name, const char *description, unsigned int options, qh_handler handler)
{
	struct query_handler *qh = NULL;
	int result = 0;

	if (name == NULL) {
		logit(NSLOG_RUNTIME_ERROR, TRUE, "qh: Failed to register handler with no name\n");
		return -1;
	}

	if (handler == NULL) {
		logit(NSLOG_RUNTIME_ERROR, TRUE, "qh: Failed to register handler '%s': No handler function specified\n", name);
		return -1;
	}

	if (strlen(name) > 128) {
		logit(NSLOG_RUNTIME_ERROR, TRUE, "qh: Failed to register handler '%s': Name too long\n", name);
		return -ENAMETOOLONG;
	}

	/* names must be unique */
	if (qh_find_handler(name)) {
		logit(NSLOG_RUNTIME_WARNING, TRUE, "qh: Handler '%s' registered more than once\n", name);
		return -1;
	}

	qh = calloc(1, sizeof(*qh));
	if (qh == NULL) {
		logit(NSLOG_RUNTIME_ERROR, TRUE, "qh: Failed to allocate memory for handler '%s'\n", name);
		return -errno;
	}

	qh->name               = name;
	qh->description        = description;
	qh->handler            = handler;
	qh->options            = options;
	qh->next_qh            = qhandlers;

	if (qhandlers) {
		qhandlers->prev_qh = qh;
	}

	qhandlers              = qh;

	result = dkhash_insert(qh_table, qh->name, NULL, qh);
	if (result < 0) {
		logit(NSLOG_RUNTIME_ERROR, TRUE,
			  "qh: Failed to insert query handler '%s' (%p) into hash table %p (%d): %s\n",
			  name, qh, qh_table, result, strerror(errno));
		free(qh);
		return result;
	}

	return 0;
}
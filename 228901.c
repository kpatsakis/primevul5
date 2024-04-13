lka_filter_process_response(const char *name, const char *line)
{
	uint64_t reqid;
	uint64_t token;
	char buffer[LINE_MAX];
	char *ep = NULL;
	char *kind = NULL;
	char *qid = NULL;
	/*char *phase = NULL;*/
	char *response = NULL;
	char *parameter = NULL;
	struct filter_session *fs;

	(void)strlcpy(buffer, line, sizeof buffer);
	if ((ep = strchr(buffer, '|')) == NULL)
		fatalx("Missing token: %s", line);
	ep[0] = '\0';

	kind = buffer;

	qid = ep+1;
	if ((ep = strchr(qid, '|')) == NULL)
		fatalx("Missing reqid: %s", line);
	ep[0] = '\0';

	reqid = strtoull(qid, &ep, 16);
	if (qid[0] == '\0' || *ep != '\0')
		fatalx("Invalid reqid: %s", line);
	if (errno == ERANGE && reqid == ULLONG_MAX)
		fatal("Invalid reqid: %s", line);

	qid = ep+1;
	if ((ep = strchr(qid, '|')) == NULL)
		fatal("Missing directive: %s", line);
	ep[0] = '\0';

	token = strtoull(qid, &ep, 16);
	if (qid[0] == '\0' || *ep != '\0')
		fatalx("Invalid token: %s", line);
	if (errno == ERANGE && token == ULLONG_MAX)
		fatal("Invalid token: %s", line);

	response = ep+1;

	/* session can legitimately disappear on a resume */
	if ((fs = tree_get(&sessions, reqid)) == NULL)
		return;

	if (strcmp(kind, "filter-dataline") == 0) {
		if (fs->phase != FILTER_DATA_LINE)
			fatalx("filter-dataline out of dataline phase");
		filter_data_next(token, reqid, response);
		return;
	}
	if (fs->phase == FILTER_DATA_LINE)
		fatalx("filter-result in dataline phase");

	if ((ep = strchr(response, '|'))) {
		parameter = ep + 1;
		ep[0] = '\0';
	}

	if (strcmp(response, "proceed") == 0) {
		if (parameter != NULL)
			fatalx("Unexpected parameter after proceed: %s", line);
		filter_protocol_next(token, reqid, 0);
		return;
	} else if (strcmp(response, "junk") == 0) {
		if (parameter != NULL)
			fatalx("Unexpected parameter after junk: %s", line);
		if (fs->phase == FILTER_COMMIT)
			fatalx("filter-reponse junk after DATA");
		filter_result_junk(reqid);
		return;
	} else {
		if (parameter == NULL)
			fatalx("Missing parameter: %s", line);

		if (strcmp(response, "rewrite") == 0)
			filter_result_rewrite(reqid, parameter);
		else if (strcmp(response, "reject") == 0)
			filter_result_reject(reqid, parameter);
		else if (strcmp(response, "disconnect") == 0)
			filter_result_disconnect(reqid, parameter);
		else
			fatalx("Invalid directive: %s", line);
	}
}
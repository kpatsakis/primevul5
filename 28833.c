static void dns_resolve_recv(struct dgram_conn *dgram)
{
	struct dns_nameserver *ns, *tmpns;
	struct dns_resolvers  *resolvers;
	struct dns_resolution *res;
	struct dns_query_item *query;
	unsigned char  buf[DNS_MAX_UDP_MESSAGE + 1];
	unsigned char *bufend;
	int fd, buflen, dns_resp;
	int max_answer_records;
	unsigned short query_id;
	struct eb32_node *eb;
	struct dns_requester *req;

	fd = dgram->t.sock.fd;

	/* check if ready for reading */
	if (!fd_recv_ready(fd))
		return;

	/* no need to go further if we can't retrieve the nameserver */
	if ((ns = dgram->owner) == NULL)
		return;

	resolvers = ns->resolvers;
	HA_SPIN_LOCK(DNS_LOCK, &resolvers->lock);

	/* process all pending input messages */
	while (1) {
		/* read message received */
		memset(buf, '\0', resolvers->accepted_payload_size + 1);
		if ((buflen = recv(fd, (char*)buf , resolvers->accepted_payload_size + 1, 0)) < 0) {
			/* FIXME : for now we consider EAGAIN only */
			fd_cant_recv(fd);
			break;
		}

		/* message too big */
		if (buflen > resolvers->accepted_payload_size) {
			ns->counters.too_big++;
			continue;
		}

		/* initializing variables */
		bufend = buf + buflen;	/* pointer to mark the end of the buffer */

		/* read the query id from the packet (16 bits) */
		if (buf + 2 > bufend) {
			ns->counters.invalid++;
			continue;
		}
		query_id = dns_response_get_query_id(buf);

		/* search the query_id in the pending resolution tree */
		eb = eb32_lookup(&resolvers->query_ids, query_id);
		if (eb == NULL) {
			/* unknown query id means an outdated response and can be safely ignored */
			ns->counters.outdated++;
			continue;
		}

		/* known query id means a resolution in prgress */
		res = eb32_entry(eb, struct dns_resolution, qid);
		if (!res) {
			ns->counters.outdated++;
			continue;
		}

		/* number of responses received */
		res->nb_responses++;

		max_answer_records = (resolvers->accepted_payload_size - DNS_HEADER_SIZE) / DNS_MIN_RECORD_SIZE;
		dns_resp = dns_validate_dns_response(buf, bufend, res, max_answer_records);

		switch (dns_resp) {
			case DNS_RESP_VALID:
				break;

			case DNS_RESP_INVALID:
			case DNS_RESP_QUERY_COUNT_ERROR:
			case DNS_RESP_WRONG_NAME:
				res->status = RSLV_STATUS_INVALID;
				ns->counters.invalid++;
				break;

			case DNS_RESP_NX_DOMAIN:
				res->status = RSLV_STATUS_NX;
				ns->counters.nx++;
				break;

			case DNS_RESP_REFUSED:
				res->status = RSLV_STATUS_REFUSED;
				ns->counters.refused++;
				break;

			case DNS_RESP_ANCOUNT_ZERO:
				res->status = RSLV_STATUS_OTHER;
				ns->counters.any_err++;
				break;

			case DNS_RESP_CNAME_ERROR:
				res->status = RSLV_STATUS_OTHER;
				ns->counters.cname_error++;
				break;

			case DNS_RESP_TRUNCATED:
				res->status = RSLV_STATUS_OTHER;
				ns->counters.truncated++;
				break;

			case DNS_RESP_NO_EXPECTED_RECORD:
			case DNS_RESP_ERROR:
			case DNS_RESP_INTERNAL:
				res->status = RSLV_STATUS_OTHER;
				ns->counters.other++;
				break;
		}

		/* Wait all nameservers response to handle errors */
		if (dns_resp != DNS_RESP_VALID && res->nb_responses < resolvers->nb_nameservers)
			continue;

		/* Process error codes */
		if (dns_resp != DNS_RESP_VALID)  {
			if (res->prefered_query_type != res->query_type) {
				/* The fallback on the query type was already performed,
				 * so check the try counter. If it falls to 0, we can
				 * report an error. Else, wait the next attempt. */
				if (!res->try)
					goto report_res_error;
			}
			else {
				/* Fallback from A to AAAA or the opposite and re-send
				 * the resolution immediately. try counter is not
				 * decremented. */
				if (res->prefered_query_type == DNS_RTYPE_A) {
					res->query_type = DNS_RTYPE_AAAA;
					dns_send_query(res);
				}
				else if (res->prefered_query_type == DNS_RTYPE_AAAA) {
					res->query_type = DNS_RTYPE_A;
					dns_send_query(res);
				}
			}
			continue;
		}

		/* Now let's check the query's dname corresponds to the one we
		 * sent. We can check only the first query of the list. We send
		 * one query at a time so we get one query in the response */
		query = LIST_NEXT(&res->response.query_list, struct dns_query_item *, list);
		if (query && memcmp(query->name, res->hostname_dn, res->hostname_dn_len) != 0) {
			dns_resp = DNS_RESP_WRONG_NAME;
			ns->counters.other++;
			goto report_res_error;
		}

		/* So the resolution succeeded */
		res->status     = RSLV_STATUS_VALID;
		res->last_valid = now_ms;
		ns->counters.valid++;
		goto report_res_success;

	report_res_error:
		list_for_each_entry(req, &res->requesters, list)
			req->requester_error_cb(req, dns_resp);
		dns_reset_resolution(res);
		LIST_DEL(&res->list);
		LIST_ADDQ(&resolvers->resolutions.wait, &res->list);
		continue;

	report_res_success:
		/* Only the 1rst requester s managed by the server, others are
		 * from the cache */
		tmpns = ns;
		list_for_each_entry(req, &res->requesters, list) {
			struct server *s = objt_server(req->owner);

			if (s)
				HA_SPIN_LOCK(SERVER_LOCK, &s->lock);
			req->requester_cb(req, tmpns);
			if (s)
				HA_SPIN_UNLOCK(SERVER_LOCK, &s->lock);
			tmpns = NULL;
		}

		dns_reset_resolution(res);
		LIST_DEL(&res->list);
		LIST_ADDQ(&resolvers->resolutions.wait, &res->list);
		continue;
	}
	dns_update_resolvers_timeout(resolvers);
	HA_SPIN_UNLOCK(DNS_LOCK, &resolvers->lock);
}

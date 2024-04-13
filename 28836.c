struct dns_srvrq *new_dns_srvrq(struct server *srv, char *fqdn)
{
	struct proxy     *px    = srv->proxy;
	struct dns_srvrq *srvrq = NULL;
	int fqdn_len, hostname_dn_len;

	fqdn_len = strlen(fqdn);
	hostname_dn_len = dns_str_to_dn_label(fqdn, fqdn_len + 1, trash.area,
					      trash.size);
	if (hostname_dn_len == -1) {
		ha_alert("config : %s '%s', server '%s': failed to parse FQDN '%s'\n",
			 proxy_type_str(px), px->id, srv->id, fqdn);
		goto err;
	}

	if ((srvrq = calloc(1, sizeof(*srvrq))) == NULL) {
		ha_alert("config : %s '%s', server '%s': out of memory\n",
			 proxy_type_str(px), px->id, srv->id);
		goto err;
	}
	srvrq->obj_type        = OBJ_TYPE_SRVRQ;
	srvrq->proxy           = px;
	srvrq->name            = strdup(fqdn);
	srvrq->hostname_dn     = strdup(trash.area);
	srvrq->hostname_dn_len = hostname_dn_len;
	if (!srvrq->name || !srvrq->hostname_dn) {
		ha_alert("config : %s '%s', server '%s': out of memory\n",
			 proxy_type_str(px), px->id, srv->id);
		goto err;
	}
	LIST_ADDQ(&dns_srvrq_list, &srvrq->list);
	return srvrq;

  err:
	if (srvrq) {
		free(srvrq->name);
		free(srvrq->hostname_dn);
		free(srvrq);
	}
	return NULL;
}

static void dns_check_dns_response(struct dns_resolution *res)
{
	struct dns_resolvers   *resolvers = res->resolvers;
	struct dns_requester   *req, *reqback;
	struct dns_answer_item *item, *itemback;
	struct server          *srv;
	struct dns_srvrq       *srvrq;

	list_for_each_entry_safe(item, itemback, &res->response.answer_list, list) {

		/* Remove obsolete items */
		if ((item->last_seen + resolvers->hold.obsolete / 1000) < now.tv_sec) {
			if (item->type != DNS_RTYPE_SRV)
				goto rm_obselete_item;

			list_for_each_entry_safe(req, reqback, &res->requesters, list) {
				if ((srvrq = objt_dns_srvrq(req->owner)) == NULL)
					continue;

				/* Remove any associated server */
				for (srv = srvrq->proxy->srv; srv != NULL; srv = srv->next) {
					HA_SPIN_LOCK(SERVER_LOCK, &srv->lock);
					if (srv->srvrq == srvrq && srv->svc_port == item->port &&
					    item->data_len == srv->hostname_dn_len &&
					    !memcmp(srv->hostname_dn, item->target, item->data_len)) {
						snr_update_srv_status(srv, 1);
						free(srv->hostname);
						free(srv->hostname_dn);
						srv->hostname        = NULL;
						srv->hostname_dn     = NULL;
						srv->hostname_dn_len = 0;
						dns_unlink_resolution(srv->dns_requester);
					}
					HA_SPIN_UNLOCK(SERVER_LOCK, &srv->lock);
				}
			}

		  rm_obselete_item:
			LIST_DEL(&item->list);
			pool_free(dns_answer_item_pool, item);
			continue;
		}

		if (item->type != DNS_RTYPE_SRV)
			continue;

		/* Now process SRV records */
		list_for_each_entry_safe(req, reqback, &res->requesters, list) {
			if ((srvrq = objt_dns_srvrq(req->owner)) == NULL)
				continue;

			/* Check if a server already uses that hostname */
			for (srv = srvrq->proxy->srv; srv != NULL; srv = srv->next) {
				HA_SPIN_LOCK(SERVER_LOCK, &srv->lock);
				if (srv->srvrq == srvrq && srv->svc_port == item->port &&
				    item->data_len == srv->hostname_dn_len &&
				    !memcmp(srv->hostname_dn, item->target, item->data_len)) {
					int ha_weight;

					/* Make sure weight is at least 1, so
					 * that the server will be used.
					 */
					ha_weight = item->weight / 256 + 1;
					if (srv->uweight != ha_weight) {
						char weight[9];

						snprintf(weight, sizeof(weight), "%d", ha_weight);
						server_parse_weight_change_request(srv, weight);
					}
					HA_SPIN_UNLOCK(SERVER_LOCK, &srv->lock);
					break;
				}
				HA_SPIN_UNLOCK(SERVER_LOCK, &srv->lock);
			}
			if (srv)
				continue;

			/* If not, try to find a server with undefined hostname */
			for (srv = srvrq->proxy->srv; srv != NULL; srv = srv->next) {
				HA_SPIN_LOCK(SERVER_LOCK, &srv->lock);
				if (srv->srvrq == srvrq && !srv->hostname_dn)
					break;
				HA_SPIN_UNLOCK(SERVER_LOCK, &srv->lock);
			}
			/* And update this server, if found */
			if (srv) {
				const char *msg = NULL;
				char weight[9];
				int ha_weight;
				char hostname[DNS_MAX_NAME_SIZE];

				if (dns_dn_label_to_str(item->target, item->data_len+1,
							hostname, DNS_MAX_NAME_SIZE) == -1) {
					HA_SPIN_UNLOCK(SERVER_LOCK, &srv->lock);
					continue;
				}
				msg = update_server_fqdn(srv, hostname, "SRV record", 1);
				if (msg)
					send_log(srv->proxy, LOG_NOTICE, "%s", msg);

				srv->svc_port = item->port;
				srv->flags   &= ~SRV_F_MAPPORTS;
				if ((srv->check.state & CHK_ST_CONFIGURED) &&
				    !(srv->flags & SRV_F_CHECKPORT))
					srv->check.port = item->port;

				/* Make sure weight is at least 1, so
				 * that the server will be used.
				 */
				ha_weight = item->weight / 256 + 1;

				snprintf(weight, sizeof(weight), "%d", ha_weight);
				server_parse_weight_change_request(srv, weight);
				HA_SPIN_UNLOCK(SERVER_LOCK, &srv->lock);
			}
		}
	}
}

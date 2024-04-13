static void dns_deinit(void)
{
	struct dns_resolvers  *resolvers, *resolversback;
	struct dns_nameserver *ns, *nsback;
	struct dns_resolution *res, *resback;
	struct dns_requester  *req, *reqback;
	struct dns_srvrq      *srvrq, *srvrqback;

	list_for_each_entry_safe(resolvers, resolversback, &dns_resolvers, list) {
		list_for_each_entry_safe(ns, nsback, &resolvers->nameservers, list) {
			free(ns->id);
			free((char *)ns->conf.file);
			if (ns->dgram && ns->dgram->t.sock.fd != -1)
				fd_delete(ns->dgram->t.sock.fd);
			free(ns->dgram);
			LIST_DEL(&ns->list);
			free(ns);
		}

		list_for_each_entry_safe(res, resback, &resolvers->resolutions.curr, list) {
			list_for_each_entry_safe(req, reqback, &res->requesters, list) {
				LIST_DEL(&req->list);
				free(req);
			}
			dns_free_resolution(res);
		}

		list_for_each_entry_safe(res, resback, &resolvers->resolutions.wait, list) {
			list_for_each_entry_safe(req, reqback, &res->requesters, list) {
				LIST_DEL(&req->list);
				free(req);
			}
			dns_free_resolution(res);
		}

		free(resolvers->id);
		free((char *)resolvers->conf.file);
		task_delete(resolvers->t);
		task_free(resolvers->t);
		LIST_DEL(&resolvers->list);
		free(resolvers);
	}

	list_for_each_entry_safe(srvrq, srvrqback, &dns_srvrq_list, list) {
		free(srvrq->name);
		free(srvrq->hostname_dn);
		LIST_DEL(&srvrq->list);
		free(srvrq);
	}
}

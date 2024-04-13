void dns_unlink_resolution(struct dns_requester *requester)
{
	struct dns_resolution *res;
	struct dns_requester  *req;

	/* Nothing to do */
	if (!requester || !requester->resolution)
		return;
	res = requester->resolution;

	/* Clean up the requester */
	LIST_DEL(&requester->list);
	requester->resolution = NULL;

	/* We need to find another requester linked on this resolution */
	if (!LIST_ISEMPTY(&res->requesters))
		req = LIST_NEXT(&res->requesters, struct dns_requester *, list);
	else {
		dns_free_resolution(res);
		return;
	}

	/* Move hostname_dn related pointers to the next requester */
	switch (obj_type(req->owner)) {
		case OBJ_TYPE_SERVER:
			res->hostname_dn     = __objt_server(req->owner)->hostname_dn;
			res->hostname_dn_len = __objt_server(req->owner)->hostname_dn_len;
			break;
		case OBJ_TYPE_SRVRQ:
			res->hostname_dn     = __objt_dns_srvrq(req->owner)->hostname_dn;
			res->hostname_dn_len = __objt_dns_srvrq(req->owner)->hostname_dn_len;
			break;
		default:
			res->hostname_dn     = NULL;
			res->hostname_dn_len = 0;
			break;
	}
}

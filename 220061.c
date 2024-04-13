int rtnl_wilddump_req_filter_fn(struct rtnl_handle *rth, int family, int type,
				req_filter_fn_t filter_fn)
{
	struct {
		struct nlmsghdr nlh;
		struct ifinfomsg ifm;
		char buf[1024];
	} req = {
		.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg)),
		.nlh.nlmsg_type = type,
		.nlh.nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST,
		.nlh.nlmsg_seq = rth->dump = ++rth->seq,
		.ifm.ifi_family = family,
	};
	int err;

	if (!filter_fn)
		return -EINVAL;

	err = filter_fn(&req.nlh, sizeof(req));
	if (err)
		return err;

	return send(rth->fd, &req, req.nlh.nlmsg_len, 0);
}
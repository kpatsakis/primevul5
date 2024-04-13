int rtnl_wilddump_stats_req_filter(struct rtnl_handle *rth, int fam, int type,
				   __u32 filt_mask)
{
	struct {
		struct nlmsghdr nlh;
		struct if_stats_msg ifsm;
	} req;

	memset(&req, 0, sizeof(req));
	req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct if_stats_msg));
	req.nlh.nlmsg_type = type;
	req.nlh.nlmsg_flags = NLM_F_DUMP|NLM_F_REQUEST;
	req.nlh.nlmsg_pid = 0;
	req.nlh.nlmsg_seq = rth->dump = ++rth->seq;
	req.ifsm.family = fam;
	req.ifsm.filter_mask = filt_mask;

	return send(rth->fd, &req, sizeof(req), 0);
}
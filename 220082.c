int rtnl_wilddump_req_filter(struct rtnl_handle *rth, int family, int type,
			    __u32 filt_mask)
{
	struct {
		struct nlmsghdr nlh;
		struct ifinfomsg ifm;
		/* attribute has to be NLMSG aligned */
		struct rtattr ext_req __attribute__ ((aligned(NLMSG_ALIGNTO)));
		__u32 ext_filter_mask;
	} req = {
		.nlh.nlmsg_len = sizeof(req),
		.nlh.nlmsg_type = type,
		.nlh.nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST,
		.nlh.nlmsg_seq = rth->dump = ++rth->seq,
		.ifm.ifi_family = family,
		.ext_req.rta_type = IFLA_EXT_MASK,
		.ext_req.rta_len = RTA_LENGTH(sizeof(__u32)),
		.ext_filter_mask = filt_mask,
	};

	return send(rth->fd, &req, sizeof(req), 0);
}
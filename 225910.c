static int route4_set_parms(struct net *net, struct tcf_proto *tp,
			    unsigned long base, struct route4_filter *f,
			    u32 handle, struct route4_head *head,
			    struct nlattr **tb, struct nlattr *est, int new,
			    bool ovr, struct netlink_ext_ack *extack)
{
	u32 id = 0, to = 0, nhandle = 0x8000;
	struct route4_filter *fp;
	unsigned int h1;
	struct route4_bucket *b;
	int err;

	err = tcf_exts_validate(net, tp, tb, est, &f->exts, ovr, true, extack);
	if (err < 0)
		return err;

	if (tb[TCA_ROUTE4_TO]) {
		if (new && handle & 0x8000)
			return -EINVAL;
		to = nla_get_u32(tb[TCA_ROUTE4_TO]);
		if (to > 0xFF)
			return -EINVAL;
		nhandle = to;
	}

	if (tb[TCA_ROUTE4_FROM]) {
		if (tb[TCA_ROUTE4_IIF])
			return -EINVAL;
		id = nla_get_u32(tb[TCA_ROUTE4_FROM]);
		if (id > 0xFF)
			return -EINVAL;
		nhandle |= id << 16;
	} else if (tb[TCA_ROUTE4_IIF]) {
		id = nla_get_u32(tb[TCA_ROUTE4_IIF]);
		if (id > 0x7FFF)
			return -EINVAL;
		nhandle |= (id | 0x8000) << 16;
	} else
		nhandle |= 0xFFFF << 16;

	if (handle && new) {
		nhandle |= handle & 0x7F00;
		if (nhandle != handle)
			return -EINVAL;
	}

	h1 = to_hash(nhandle);
	b = rtnl_dereference(head->table[h1]);
	if (!b) {
		b = kzalloc(sizeof(struct route4_bucket), GFP_KERNEL);
		if (b == NULL)
			return -ENOBUFS;

		rcu_assign_pointer(head->table[h1], b);
	} else {
		unsigned int h2 = from_hash(nhandle >> 16);

		for (fp = rtnl_dereference(b->ht[h2]);
		     fp;
		     fp = rtnl_dereference(fp->next))
			if (fp->handle == f->handle)
				return -EEXIST;
	}

	if (tb[TCA_ROUTE4_TO])
		f->id = to;

	if (tb[TCA_ROUTE4_FROM])
		f->id = to | id<<16;
	else if (tb[TCA_ROUTE4_IIF])
		f->iif = id;

	f->handle = nhandle;
	f->bkt = b;
	f->tp = tp;

	if (tb[TCA_ROUTE4_CLASSID]) {
		f->res.classid = nla_get_u32(tb[TCA_ROUTE4_CLASSID]);
		tcf_bind_filter(tp, &f->res, base);
	}

	return 0;
}
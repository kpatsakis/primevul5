ctnetlink_create_conntrack(struct net *net,
			   const struct nf_conntrack_zone *zone,
			   const struct nlattr * const cda[],
			   struct nf_conntrack_tuple *otuple,
			   struct nf_conntrack_tuple *rtuple,
			   u8 u3)
{
	struct nf_conn *ct;
	int err = -EINVAL;
	struct nf_conntrack_helper *helper;
	struct nf_conn_tstamp *tstamp;
	u64 timeout;

	ct = nf_conntrack_alloc(net, zone, otuple, rtuple, GFP_ATOMIC);
	if (IS_ERR(ct))
		return ERR_PTR(-ENOMEM);

	if (!cda[CTA_TIMEOUT])
		goto err1;

	timeout = (u64)ntohl(nla_get_be32(cda[CTA_TIMEOUT])) * HZ;
	if (timeout > INT_MAX)
		timeout = INT_MAX;
	ct->timeout = (u32)timeout + nfct_time_stamp;

	rcu_read_lock();
 	if (cda[CTA_HELP]) {
		char *helpname = NULL;
		struct nlattr *helpinfo = NULL;

		err = ctnetlink_parse_help(cda[CTA_HELP], &helpname, &helpinfo);
 		if (err < 0)
			goto err2;

		helper = __nf_conntrack_helper_find(helpname, nf_ct_l3num(ct),
						    nf_ct_protonum(ct));
		if (helper == NULL) {
			rcu_read_unlock();
#ifdef CONFIG_MODULES
			if (request_module("nfct-helper-%s", helpname) < 0) {
				err = -EOPNOTSUPP;
				goto err1;
			}

			rcu_read_lock();
			helper = __nf_conntrack_helper_find(helpname,
							    nf_ct_l3num(ct),
							    nf_ct_protonum(ct));
			if (helper) {
				err = -EAGAIN;
				goto err2;
			}
			rcu_read_unlock();
#endif
			err = -EOPNOTSUPP;
			goto err1;
		} else {
			struct nf_conn_help *help;

			help = nf_ct_helper_ext_add(ct, GFP_ATOMIC);
			if (help == NULL) {
				err = -ENOMEM;
				goto err2;
			}
			/* set private helper data if allowed. */
			if (helper->from_nlattr)
				helper->from_nlattr(helpinfo, ct);

			/* not in hash table yet so not strictly necessary */
			RCU_INIT_POINTER(help->helper, helper);
		}
	} else {
		/* try an implicit helper assignation */
		err = __nf_ct_try_assign_helper(ct, NULL, GFP_ATOMIC);
		if (err < 0)
			goto err2;
	}

	err = ctnetlink_setup_nat(ct, cda);
	if (err < 0)
		goto err2;

	nf_ct_acct_ext_add(ct, GFP_ATOMIC);
	nf_ct_tstamp_ext_add(ct, GFP_ATOMIC);
	nf_ct_ecache_ext_add(ct, 0, 0, GFP_ATOMIC);
	nf_ct_labels_ext_add(ct);
	nfct_seqadj_ext_add(ct);
	nfct_synproxy_ext_add(ct);

	/* we must add conntrack extensions before confirmation. */
	ct->status |= IPS_CONFIRMED;

	if (cda[CTA_STATUS]) {
		err = ctnetlink_change_status(ct, cda);
		if (err < 0)
			goto err2;
	}

	if (cda[CTA_SEQ_ADJ_ORIG] || cda[CTA_SEQ_ADJ_REPLY]) {
		err = ctnetlink_change_seq_adj(ct, cda);
		if (err < 0)
			goto err2;
	}

	memset(&ct->proto, 0, sizeof(ct->proto));
	if (cda[CTA_PROTOINFO]) {
		err = ctnetlink_change_protoinfo(ct, cda);
		if (err < 0)
			goto err2;
	}

	if (cda[CTA_SYNPROXY]) {
		err = ctnetlink_change_synproxy(ct, cda);
		if (err < 0)
			goto err2;
	}

#if defined(CONFIG_NF_CONNTRACK_MARK)
	if (cda[CTA_MARK])
		ctnetlink_change_mark(ct, cda);
#endif

	/* setup master conntrack: this is a confirmed expectation */
	if (cda[CTA_TUPLE_MASTER]) {
		struct nf_conntrack_tuple master;
		struct nf_conntrack_tuple_hash *master_h;
		struct nf_conn *master_ct;

		err = ctnetlink_parse_tuple(cda, &master, CTA_TUPLE_MASTER,
					    u3, NULL);
		if (err < 0)
			goto err2;

		master_h = nf_conntrack_find_get(net, zone, &master);
		if (master_h == NULL) {
			err = -ENOENT;
			goto err2;
		}
		master_ct = nf_ct_tuplehash_to_ctrack(master_h);
		__set_bit(IPS_EXPECTED_BIT, &ct->status);
		ct->master = master_ct;
	}
	tstamp = nf_conn_tstamp_find(ct);
	if (tstamp)
		tstamp->start = ktime_get_real_ns();

	err = nf_conntrack_hash_check_insert(ct);
	if (err < 0)
		goto err2;

	rcu_read_unlock();

	return ct;

err2:
	rcu_read_unlock();
err1:
	nf_conntrack_free(ct);
	return ERR_PTR(err);
}
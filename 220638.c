static int fanout_add(struct sock *sk, u16 id, u16 type_flags)
{
	struct packet_rollover *rollover = NULL;
	struct packet_sock *po = pkt_sk(sk);
	struct packet_fanout *f, *match;
	u8 type = type_flags & 0xff;
	u8 flags = type_flags >> 8;
	int err;

	switch (type) {
	case PACKET_FANOUT_ROLLOVER:
		if (type_flags & PACKET_FANOUT_FLAG_ROLLOVER)
			return -EINVAL;
	case PACKET_FANOUT_HASH:
	case PACKET_FANOUT_LB:
	case PACKET_FANOUT_CPU:
	case PACKET_FANOUT_RND:
	case PACKET_FANOUT_QM:
	case PACKET_FANOUT_CBPF:
	case PACKET_FANOUT_EBPF:
		break;
	default:
		return -EINVAL;
	}

	mutex_lock(&fanout_mutex);

	err = -EALREADY;
	if (po->fanout)
		goto out;

	if (type == PACKET_FANOUT_ROLLOVER ||
	    (type_flags & PACKET_FANOUT_FLAG_ROLLOVER)) {
		err = -ENOMEM;
		rollover = kzalloc(sizeof(*rollover), GFP_KERNEL);
		if (!rollover)
			goto out;
		atomic_long_set(&rollover->num, 0);
		atomic_long_set(&rollover->num_huge, 0);
		atomic_long_set(&rollover->num_failed, 0);
	}

	if (type_flags & PACKET_FANOUT_FLAG_UNIQUEID) {
		if (id != 0) {
			err = -EINVAL;
			goto out;
		}
		if (!fanout_find_new_id(sk, &id)) {
			err = -ENOMEM;
			goto out;
		}
		/* ephemeral flag for the first socket in the group: drop it */
		flags &= ~(PACKET_FANOUT_FLAG_UNIQUEID >> 8);
	}

	match = NULL;
	list_for_each_entry(f, &fanout_list, list) {
		if (f->id == id &&
		    read_pnet(&f->net) == sock_net(sk)) {
			match = f;
			break;
		}
	}
	err = -EINVAL;
	if (match && match->flags != flags)
		goto out;
	if (!match) {
		err = -ENOMEM;
		match = kzalloc(sizeof(*match), GFP_KERNEL);
		if (!match)
			goto out;
		write_pnet(&match->net, sock_net(sk));
		match->id = id;
		match->type = type;
		match->flags = flags;
		INIT_LIST_HEAD(&match->list);
		spin_lock_init(&match->lock);
		refcount_set(&match->sk_ref, 0);
		fanout_init_data(match);
		match->prot_hook.type = po->prot_hook.type;
		match->prot_hook.dev = po->prot_hook.dev;
		match->prot_hook.func = packet_rcv_fanout;
		match->prot_hook.af_packet_priv = match;
		match->prot_hook.id_match = match_fanout_group;
		list_add(&match->list, &fanout_list);
	}
	err = -EINVAL;

	spin_lock(&po->bind_lock);
	if (po->running &&
	    match->type == type &&
	    match->prot_hook.type == po->prot_hook.type &&
	    match->prot_hook.dev == po->prot_hook.dev) {
		err = -ENOSPC;
		if (refcount_read(&match->sk_ref) < PACKET_FANOUT_MAX) {
			__dev_remove_pack(&po->prot_hook);
			po->fanout = match;
			po->rollover = rollover;
			rollover = NULL;
			refcount_set(&match->sk_ref, refcount_read(&match->sk_ref) + 1);
			__fanout_link(sk, po);
			err = 0;
		}
	}
	spin_unlock(&po->bind_lock);

	if (err && !refcount_read(&match->sk_ref)) {
		list_del(&match->list);
		kfree(match);
	}

out:
	kfree(rollover);
	mutex_unlock(&fanout_mutex);
	return err;
}
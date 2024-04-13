static int sctp_setsockopt_auto_asconf(struct sock *sk, int *val,
					unsigned int optlen)
{
	struct sctp_sock *sp = sctp_sk(sk);

	if (optlen < sizeof(int))
		return -EINVAL;
	if (!sctp_is_ep_boundall(sk) && *val)
		return -EINVAL;
	if ((*val && sp->do_auto_asconf) || (!*val && !sp->do_auto_asconf))
		return 0;

	spin_lock_bh(&sock_net(sk)->sctp.addr_wq_lock);
	if (*val == 0 && sp->do_auto_asconf) {
		list_del(&sp->auto_asconf_list);
		sp->do_auto_asconf = 0;
	} else if (*val && !sp->do_auto_asconf) {
		list_add_tail(&sp->auto_asconf_list,
		    &sock_net(sk)->sctp.auto_asconf_splist);
		sp->do_auto_asconf = 1;
	}
	spin_unlock_bh(&sock_net(sk)->sctp.addr_wq_lock);
	return 0;
}
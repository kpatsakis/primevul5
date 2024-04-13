static void __unregister_prot_hook(struct sock *sk, bool sync)
{
	struct packet_sock *po = pkt_sk(sk);

	lockdep_assert_held_once(&po->bind_lock);

	po->running = 0;

	if (po->fanout)
		__fanout_unlink(sk, po);
	else
		__dev_remove_pack(&po->prot_hook);

	__sock_put(sk);

	if (sync) {
		spin_unlock(&po->bind_lock);
		synchronize_net();
		spin_lock(&po->bind_lock);
	}
}
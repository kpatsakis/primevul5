 */
int sock_queue_err_skb(struct sock *sk, struct sk_buff *skb)
{
	if (atomic_read(&sk->sk_rmem_alloc) + skb->truesize >=
	    (unsigned int)sk->sk_rcvbuf)
		return -ENOMEM;

	skb_orphan(skb);
	skb->sk = sk;
	skb->destructor = sock_rmem_free;
	atomic_add(skb->truesize, &sk->sk_rmem_alloc);

	/* before exiting rcu section, make sure dst is refcounted */
	skb_dst_force(skb);

	skb_queue_tail(&sk->sk_error_queue, skb);
	if (!sock_flag(sk, SOCK_DEAD))
		sk->sk_data_ready(sk);
	return 0;
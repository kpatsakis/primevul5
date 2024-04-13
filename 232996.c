struct sk_buff *ip6_make_skb(struct sock *sk,
			     int getfrag(void *from, char *to, int offset,
					 int len, int odd, struct sk_buff *skb),
			     void *from, int length, int transhdrlen,
			     struct ipcm6_cookie *ipc6, struct flowi6 *fl6,
			     struct rt6_info *rt, unsigned int flags,
			     const struct sockcm_cookie *sockc)
{
	struct inet_cork_full cork;
	struct inet6_cork v6_cork;
	struct sk_buff_head queue;
	int exthdrlen = (ipc6->opt ? ipc6->opt->opt_flen : 0);
	int err;

	if (flags & MSG_PROBE)
		return NULL;

	__skb_queue_head_init(&queue);

	cork.base.flags = 0;
	cork.base.addr = 0;
	cork.base.opt = NULL;
	v6_cork.opt = NULL;
	err = ip6_setup_cork(sk, &cork, &v6_cork, ipc6, rt, fl6);
	if (err)
		return ERR_PTR(err);

	if (ipc6->dontfrag < 0)
		ipc6->dontfrag = inet6_sk(sk)->dontfrag;

	err = __ip6_append_data(sk, fl6, &queue, &cork.base, &v6_cork,
				&current->task_frag, getfrag, from,
				length + exthdrlen, transhdrlen + exthdrlen,
				flags, ipc6, sockc);
	if (err) {
		__ip6_flush_pending_frames(sk, &queue, &cork, &v6_cork);
		return ERR_PTR(err);
	}

	return __ip6_make_skb(sk, &queue, &cork, &v6_cork);
}
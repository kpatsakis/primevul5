static int rose_recvmsg(struct kiocb *iocb, struct socket *sock,
			struct msghdr *msg, size_t size, int flags)
{
	struct sock *sk = sock->sk;
	struct rose_sock *rose = rose_sk(sk);
	struct sockaddr_rose *srose = (struct sockaddr_rose *)msg->msg_name;
	size_t copied;
	unsigned char *asmptr;
	struct sk_buff *skb;
	int n, er, qbit;

	/*
	 * This works for seqpacket too. The receiver has ordered the queue for
	 * us! We do one quick check first though
	 */
	if (sk->sk_state != TCP_ESTABLISHED)
		return -ENOTCONN;

	/* Now we can treat all alike */
	if ((skb = skb_recv_datagram(sk, flags & ~MSG_DONTWAIT, flags & MSG_DONTWAIT, &er)) == NULL)
		return er;

	qbit = (skb->data[0] & ROSE_Q_BIT) == ROSE_Q_BIT;

	skb_pull(skb, ROSE_MIN_LEN);

	if (rose->qbitincl) {
		asmptr  = skb_push(skb, 1);
		*asmptr = qbit;
	}

	skb_reset_transport_header(skb);
	copied     = skb->len;

	if (copied > size) {
		copied = size;
		msg->msg_flags |= MSG_TRUNC;
	}

 	skb_copy_datagram_iovec(skb, 0, msg->msg_iov, copied);
 
 	if (srose != NULL) {
 		srose->srose_family = AF_ROSE;
 		srose->srose_addr   = rose->dest_addr;
 		srose->srose_call   = rose->dest_call;
		srose->srose_ndigis = rose->dest_ndigis;
		if (msg->msg_namelen >= sizeof(struct full_sockaddr_rose)) {
			struct full_sockaddr_rose *full_srose = (struct full_sockaddr_rose *)msg->msg_name;
			for (n = 0 ; n < rose->dest_ndigis ; n++)
				full_srose->srose_digis[n] = rose->dest_digis[n];
			msg->msg_namelen = sizeof(struct full_sockaddr_rose);
		} else {
			if (rose->dest_ndigis >= 1) {
				srose->srose_ndigis = 1;
				srose->srose_digi = rose->dest_digis[0];
			}
			msg->msg_namelen = sizeof(struct sockaddr_rose);
		}
	}

	skb_free_datagram(sk, skb);

	return copied;
}

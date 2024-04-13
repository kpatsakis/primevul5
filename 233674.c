static unsigned int irda_poll(struct file * file, struct socket *sock,
			      poll_table *wait)
{
	struct sock *sk = sock->sk;
	struct irda_sock *self = irda_sk(sk);
	unsigned int mask;

	IRDA_DEBUG(4, "%s()\n", __func__);

	poll_wait(file, sk->sk_sleep, wait);
	mask = 0;

	/* Exceptional events? */
	if (sk->sk_err)
		mask |= POLLERR;
	if (sk->sk_shutdown & RCV_SHUTDOWN) {
		IRDA_DEBUG(0, "%s(), POLLHUP\n", __func__);
		mask |= POLLHUP;
	}

	/* Readable? */
	if (!skb_queue_empty(&sk->sk_receive_queue)) {
		IRDA_DEBUG(4, "Socket is readable\n");
		mask |= POLLIN | POLLRDNORM;
	}

	/* Connection-based need to check for termination and startup */
	switch (sk->sk_type) {
	case SOCK_STREAM:
		if (sk->sk_state == TCP_CLOSE) {
			IRDA_DEBUG(0, "%s(), POLLHUP\n", __func__);
			mask |= POLLHUP;
		}

		if (sk->sk_state == TCP_ESTABLISHED) {
			if ((self->tx_flow == FLOW_START) &&
			    sock_writeable(sk))
			{
				mask |= POLLOUT | POLLWRNORM | POLLWRBAND;
			}
		}
		break;
	case SOCK_SEQPACKET:
		if ((self->tx_flow == FLOW_START) &&
		    sock_writeable(sk))
		{
			mask |= POLLOUT | POLLWRNORM | POLLWRBAND;
		}
		break;
	case SOCK_DGRAM:
		if (sock_writeable(sk))
			mask |= POLLOUT | POLLWRNORM | POLLWRBAND;
		break;
	default:
		break;
	}
	return mask;
}
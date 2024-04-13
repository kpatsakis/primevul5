static int irda_shutdown(struct socket *sock, int how)
{
	struct sock *sk = sock->sk;
	struct irda_sock *self = irda_sk(sk);

	IRDA_DEBUG(1, "%s(%p)\n", __func__, self);

	sk->sk_state       = TCP_CLOSE;
	sk->sk_shutdown   |= SEND_SHUTDOWN;
	sk->sk_state_change(sk);

	if (self->iriap) {
		iriap_close(self->iriap);
		self->iriap = NULL;
	}

	if (self->tsap) {
		irttp_disconnect_request(self->tsap, NULL, P_NORMAL);
		irttp_close_tsap(self->tsap);
		self->tsap = NULL;
	}

	/* A few cleanup so the socket look as good as new... */
	self->rx_flow = self->tx_flow = FLOW_START;	/* needed ??? */
	self->daddr = DEV_ADDR_ANY;	/* Until we get re-connected */
	self->saddr = 0x0;		/* so IrLMP assign us any link */

	return 0;
}
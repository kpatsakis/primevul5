static int irda_connect(struct socket *sock, struct sockaddr *uaddr,
			int addr_len, int flags)
{
	struct sock *sk = sock->sk;
	struct sockaddr_irda *addr = (struct sockaddr_irda *) uaddr;
	struct irda_sock *self = irda_sk(sk);
	int err;

	IRDA_DEBUG(2, "%s(%p)\n", __func__, self);

	/* Don't allow connect for Ultra sockets */
	if ((sk->sk_type == SOCK_DGRAM) && (sk->sk_protocol == IRDAPROTO_ULTRA))
		return -ESOCKTNOSUPPORT;

	if (sk->sk_state == TCP_ESTABLISHED && sock->state == SS_CONNECTING) {
		sock->state = SS_CONNECTED;
		return 0;   /* Connect completed during a ERESTARTSYS event */
	}

	if (sk->sk_state == TCP_CLOSE && sock->state == SS_CONNECTING) {
		sock->state = SS_UNCONNECTED;
		return -ECONNREFUSED;
	}

	if (sk->sk_state == TCP_ESTABLISHED)
		return -EISCONN;      /* No reconnect on a seqpacket socket */

	sk->sk_state   = TCP_CLOSE;
	sock->state = SS_UNCONNECTED;

	if (addr_len != sizeof(struct sockaddr_irda))
		return -EINVAL;

	/* Check if user supplied any destination device address */
	if ((!addr->sir_addr) || (addr->sir_addr == DEV_ADDR_ANY)) {
		/* Try to find one suitable */
		err = irda_discover_daddr_and_lsap_sel(self, addr->sir_name);
		if (err) {
			IRDA_DEBUG(0, "%s(), auto-connect failed!\n", __func__);
			return err;
		}
	} else {
		/* Use the one provided by the user */
		self->daddr = addr->sir_addr;
		IRDA_DEBUG(1, "%s(), daddr = %08x\n", __func__, self->daddr);

		/* If we don't have a valid service name, we assume the
		 * user want to connect on a specific LSAP. Prevent
		 * the use of invalid LSAPs (IrLMP 1.1 p10). Jean II */
		if((addr->sir_name[0] != '\0') ||
		   (addr->sir_lsap_sel >= 0x70)) {
			/* Query remote LM-IAS using service name */
			err = irda_find_lsap_sel(self, addr->sir_name);
			if (err) {
				IRDA_DEBUG(0, "%s(), connect failed!\n", __func__);
				return err;
			}
		} else {
			/* Directly connect to the remote LSAP
			 * specified by the sir_lsap field.
			 * Please use with caution, in IrDA LSAPs are
			 * dynamic and there is no "well-known" LSAP. */
			self->dtsap_sel = addr->sir_lsap_sel;
		}
	}

	/* Check if we have opened a local TSAP */
	if (!self->tsap)
		irda_open_tsap(self, LSAP_ANY, addr->sir_name);

	/* Move to connecting socket, start sending Connect Requests */
	sock->state = SS_CONNECTING;
	sk->sk_state   = TCP_SYN_SENT;

	/* Connect to remote device */
	err = irttp_connect_request(self->tsap, self->dtsap_sel,
				    self->saddr, self->daddr, NULL,
				    self->max_sdu_size_rx, NULL);
	if (err) {
		IRDA_DEBUG(0, "%s(), connect failed!\n", __func__);
		return err;
	}

	/* Now the loop */
	if (sk->sk_state != TCP_ESTABLISHED && (flags & O_NONBLOCK))
		return -EINPROGRESS;

	if (wait_event_interruptible(*(sk->sk_sleep),
				     (sk->sk_state != TCP_SYN_SENT)))
		return -ERESTARTSYS;

	if (sk->sk_state != TCP_ESTABLISHED) {
		sock->state = SS_UNCONNECTED;
		err = sock_error(sk);
		return err? err : -ECONNRESET;
	}

	sock->state = SS_CONNECTED;

	/* At this point, IrLMP has assigned our source address */
	self->saddr = irttp_get_saddr(self->tsap);

	return 0;
}
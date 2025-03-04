static int irda_bind(struct socket *sock, struct sockaddr *uaddr, int addr_len)
{
	struct sock *sk = sock->sk;
	struct sockaddr_irda *addr = (struct sockaddr_irda *) uaddr;
	struct irda_sock *self = irda_sk(sk);
	int err;

	IRDA_DEBUG(2, "%s(%p)\n", __func__, self);

	if (addr_len != sizeof(struct sockaddr_irda))
		return -EINVAL;

#ifdef CONFIG_IRDA_ULTRA
	/* Special care for Ultra sockets */
	if ((sk->sk_type == SOCK_DGRAM) &&
	    (sk->sk_protocol == IRDAPROTO_ULTRA)) {
		self->pid = addr->sir_lsap_sel;
		if (self->pid & 0x80) {
			IRDA_DEBUG(0, "%s(), extension in PID not supp!\n", __func__);
			return -EOPNOTSUPP;
		}
		err = irda_open_lsap(self, self->pid);
		if (err < 0)
			return err;

		/* Pretend we are connected */
		sock->state = SS_CONNECTED;
		sk->sk_state   = TCP_ESTABLISHED;

		return 0;
	}
#endif /* CONFIG_IRDA_ULTRA */

	self->ias_obj = irias_new_object(addr->sir_name, jiffies);
	if (self->ias_obj == NULL)
		return -ENOMEM;

	err = irda_open_tsap(self, addr->sir_lsap_sel, addr->sir_name);
	if (err < 0) {
		kfree(self->ias_obj->name);
		kfree(self->ias_obj);
		return err;
	}

	/*  Register with LM-IAS */
	irias_add_integer_attrib(self->ias_obj, "IrDA:TinyTP:LsapSel",
				 self->stsap_sel, IAS_KERNEL_ATTR);
	irias_insert_object(self->ias_obj);

	return 0;
}
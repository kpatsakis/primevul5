static void irda_flow_indication(void *instance, void *sap, LOCAL_FLOW flow)
{
	struct irda_sock *self;
	struct sock *sk;

	IRDA_DEBUG(2, "%s()\n", __func__);

	self = instance;
	sk = instance;
	BUG_ON(sk == NULL);

	switch (flow) {
	case FLOW_STOP:
		IRDA_DEBUG(1, "%s(), IrTTP wants us to slow down\n",
			   __func__);
		self->tx_flow = flow;
		break;
	case FLOW_START:
		self->tx_flow = flow;
		IRDA_DEBUG(1, "%s(), IrTTP wants us to start again\n",
			   __func__);
		wake_up_interruptible(sk->sk_sleep);
		break;
	default:
		IRDA_DEBUG(0, "%s(), Unknown flow command!\n", __func__);
		/* Unknown flow command, better stop */
		self->tx_flow = flow;
		break;
	}
}
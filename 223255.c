void xdp_add_sk_umem(struct xdp_umem *umem, struct xdp_sock *xs)
{
	unsigned long flags;

	if (!xs->tx)
		return;

	spin_lock_irqsave(&umem->xsk_list_lock, flags);
	list_add_rcu(&xs->list, &umem->xsk_list);
	spin_unlock_irqrestore(&umem->xsk_list_lock, flags);
}
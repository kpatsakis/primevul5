 * iscsi interface functions
 */
static struct iscsi_internal *
iscsi_if_transport_lookup(struct iscsi_transport *tt)
{
	struct iscsi_internal *priv;
	unsigned long flags;

	spin_lock_irqsave(&iscsi_transport_lock, flags);
	list_for_each_entry(priv, &iscsi_transports, list) {
		if (tt == priv->iscsi_transport) {
			spin_unlock_irqrestore(&iscsi_transport_lock, flags);
			return priv;
		}
	}
}
EXPORT_SYMBOL_GPL(iscsi_session_chkready);

int iscsi_is_session_online(struct iscsi_cls_session *session)
{
	unsigned long flags;
	int ret = 0;

	spin_lock_irqsave(&session->lock, flags);
	if (session->state == ISCSI_SESSION_LOGGED_IN)
		ret = 1;
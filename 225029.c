}
EXPORT_SYMBOL_GPL(iscsi_block_scsi_eh);

static void session_recovery_timedout(struct work_struct *work)
{
	struct iscsi_cls_session *session =
		container_of(work, struct iscsi_cls_session,
			     recovery_work.work);
	unsigned long flags;

	iscsi_cls_session_printk(KERN_INFO, session,
				 "session recovery timed out after %d secs\n",
				 session->recovery_tmo);

	spin_lock_irqsave(&session->lock, flags);
	switch (session->state) {
	case ISCSI_SESSION_FAILED:
		session->state = ISCSI_SESSION_FREE;
		break;
	case ISCSI_SESSION_LOGGED_IN:
	case ISCSI_SESSION_FREE:
		/* we raced with the unblock's flush */
		spin_unlock_irqrestore(&session->lock, flags);
		return;
	}
	spin_unlock_irqrestore(&session->lock, flags);

	if (session->transport->session_recovery_timedout)
		session->transport->session_recovery_timedout(session);

	ISCSI_DBG_TRANS_SESSION(session, "Unblocking SCSI target\n");
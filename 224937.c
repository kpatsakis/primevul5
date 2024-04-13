}
EXPORT_SYMBOL_GPL(iscsi_unblock_session);

static void __iscsi_block_session(struct work_struct *work)
{
	struct iscsi_cls_session *session =
			container_of(work, struct iscsi_cls_session,
				     block_work);
	unsigned long flags;

	ISCSI_DBG_TRANS_SESSION(session, "Blocking session\n");
	spin_lock_irqsave(&session->lock, flags);
	session->state = ISCSI_SESSION_FAILED;
	spin_unlock_irqrestore(&session->lock, flags);
	scsi_target_block(&session->dev);
	ISCSI_DBG_TRANS_SESSION(session, "Completed SCSI target blocking\n");
	if (session->recovery_tmo >= 0)
		queue_delayed_work(iscsi_eh_timer_workq,
	ISCSI_DBG_TRANS_SESSION(session, "Completed unblocking SCSI target\n");
}

static void __iscsi_unblock_session(struct work_struct *work)
{
	struct iscsi_cls_session *session =
			container_of(work, struct iscsi_cls_session,
				     unblock_work);
	struct Scsi_Host *shost = iscsi_session_to_shost(session);
	struct iscsi_cls_host *ihost = shost->shost_data;
	unsigned long flags;

	ISCSI_DBG_TRANS_SESSION(session, "Unblocking session\n");
	/*
	 * The recovery and unblock work get run from the same workqueue,
	 * so try to cancel it if it was going to run after this unblock.
	 */
	cancel_delayed_work(&session->recovery_work);
	spin_lock_irqsave(&session->lock, flags);
	session->state = ISCSI_SESSION_LOGGED_IN;
	spin_unlock_irqrestore(&session->lock, flags);
	/* start IO */
	scsi_target_unblock(&session->dev, SDEV_RUNNING);
	/*
	 * Only do kernel scanning if the driver is properly hooked into
	 * the async scanning code (drivers like iscsi_tcp do login and
	 * scanning from userspace).
	 */
	if (shost->hostt->scan_finished) {
		if (scsi_queue_work(shost, &session->scan_work))
			atomic_inc(&ihost->nr_scans);
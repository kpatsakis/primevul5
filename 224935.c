}
EXPORT_SYMBOL_GPL(iscsi_block_session);

static void __iscsi_unbind_session(struct work_struct *work)
{
	struct iscsi_cls_session *session =
			container_of(work, struct iscsi_cls_session,
				     unbind_work);
	struct Scsi_Host *shost = iscsi_session_to_shost(session);
	struct iscsi_cls_host *ihost = shost->shost_data;
	unsigned long flags;
	unsigned int target_id;

	ISCSI_DBG_TRANS_SESSION(session, "Unbinding session\n");

	/* Prevent new scans and make sure scanning is not in progress */
	mutex_lock(&ihost->mutex);
	spin_lock_irqsave(&session->lock, flags);
	if (session->target_id == ISCSI_MAX_TARGET) {
		spin_unlock_irqrestore(&session->lock, flags);
		mutex_unlock(&ihost->mutex);
		goto unbind_session_exit;
	}

	target_id = session->target_id;
	session->target_id = ISCSI_MAX_TARGET;
	spin_unlock_irqrestore(&session->lock, flags);
	mutex_unlock(&ihost->mutex);

	scsi_remove_target(&session->dev);

	if (session->ida_used)
		ida_simple_remove(&iscsi_sess_ida, target_id);

unbind_session_exit:
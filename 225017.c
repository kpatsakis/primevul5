	enum scsi_scan_mode rescan;
};

static int iscsi_user_scan_session(struct device *dev, void *data)
{
	struct iscsi_scan_data *scan_data = data;
	struct iscsi_cls_session *session;
	struct Scsi_Host *shost;
	struct iscsi_cls_host *ihost;
	unsigned long flags;
	unsigned int id;

	if (!iscsi_is_session_dev(dev))
		return 0;

	session = iscsi_dev_to_session(dev);

	ISCSI_DBG_TRANS_SESSION(session, "Scanning session\n");

	shost = iscsi_session_to_shost(session);
	ihost = shost->shost_data;

	mutex_lock(&ihost->mutex);
	spin_lock_irqsave(&session->lock, flags);
	if (session->state != ISCSI_SESSION_LOGGED_IN) {
		spin_unlock_irqrestore(&session->lock, flags);
		goto user_scan_exit;
	}
	id = session->target_id;
	spin_unlock_irqrestore(&session->lock, flags);

	if (id != ISCSI_MAX_TARGET) {
		if ((scan_data->channel == SCAN_WILD_CARD ||
		     scan_data->channel == 0) &&
		    (scan_data->id == SCAN_WILD_CARD ||
		     scan_data->id == id))
			scsi_scan_target(&session->dev, 0, id,
					 scan_data->lun, scan_data->rescan);
	}

user_scan_exit:
	mutex_unlock(&ihost->mutex);
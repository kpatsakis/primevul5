				     iscsi_user_scan_session);
}

static void iscsi_scan_session(struct work_struct *work)
{
	struct iscsi_cls_session *session =
			container_of(work, struct iscsi_cls_session, scan_work);
	struct Scsi_Host *shost = iscsi_session_to_shost(session);
	struct iscsi_cls_host *ihost = shost->shost_data;
	struct iscsi_scan_data scan_data;

	scan_data.channel = 0;
	scan_data.id = SCAN_WILD_CARD;
	scan_data.lun = SCAN_WILD_CARD;
	scan_data.rescan = SCSI_SCAN_RESCAN;

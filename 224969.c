}

struct iscsi_cls_session *
iscsi_alloc_session(struct Scsi_Host *shost, struct iscsi_transport *transport,
		    int dd_size)
{
	struct iscsi_cls_session *session;

	session = kzalloc(sizeof(*session) + dd_size,
			  GFP_KERNEL);
	if (!session)
		return NULL;

	session->transport = transport;
	session->creator = -1;
	session->recovery_tmo = 120;
	session->recovery_tmo_sysfs_override = false;
	session->state = ISCSI_SESSION_FREE;
	INIT_DELAYED_WORK(&session->recovery_work, session_recovery_timedout);
	INIT_LIST_HEAD(&session->sess_list);
	INIT_WORK(&session->unblock_work, __iscsi_unblock_session);
	INIT_WORK(&session->block_work, __iscsi_block_session);
	INIT_WORK(&session->unbind_work, __iscsi_unbind_session);
	INIT_WORK(&session->scan_work, iscsi_scan_session);
	INIT_WORK(&session->destroy_work, __iscsi_destroy_session);
	spin_lock_init(&session->lock);

	/* this is released in the dev's release function */
	scsi_host_get(shost);
	session->dev.parent = &shost->shost_gendev;
	session->dev.release = iscsi_session_release;
	device_initialize(&session->dev);
	if (dd_size)
		session->dd_data = &session[1];

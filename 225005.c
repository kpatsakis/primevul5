	return iscsi_destroy_conn(iscsi_dev_to_conn(dev));
}

void iscsi_remove_session(struct iscsi_cls_session *session)
{
	unsigned long flags;
	int err;

	ISCSI_DBG_TRANS_SESSION(session, "Removing session\n");

	spin_lock_irqsave(&sesslock, flags);
	if (!list_empty(&session->sess_list))
		list_del(&session->sess_list);
	spin_unlock_irqrestore(&sesslock, flags);

	flush_work(&session->block_work);
	flush_work(&session->unblock_work);
	cancel_delayed_work_sync(&session->recovery_work);
	/*
	 * If we are blocked let commands flow again. The lld or iscsi
	 * layer should set up the queuecommand to fail commands.
	 * We assume that LLD will not be calling block/unblock while
	 * removing the session.
	 */
	spin_lock_irqsave(&session->lock, flags);
	session->state = ISCSI_SESSION_FREE;
	spin_unlock_irqrestore(&session->lock, flags);

	scsi_target_unblock(&session->dev, SDEV_TRANSPORT_OFFLINE);
	/* flush running scans then delete devices */
	flush_work(&session->scan_work);
	/* flush running unbind operations */
	flush_work(&session->unbind_work);
	__iscsi_unbind_session(&session->unbind_work);

	/* hw iscsi may not have removed all connections from session */
	err = device_for_each_child(&session->dev, NULL,
				    iscsi_iter_destroy_conn_fn);
	if (err)
		iscsi_cls_session_printk(KERN_ERR, session,
					 "Could not delete all connections "
					 "for session. Error %d.\n", err);

	transport_unregister_device(&session->dev);

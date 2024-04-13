}
EXPORT_SYMBOL_GPL(iscsi_alloc_session);

int iscsi_add_session(struct iscsi_cls_session *session, unsigned int target_id)
{
	unsigned long flags;
	int id = 0;
	int err;

	session->sid = atomic_add_return(1, &iscsi_session_nr);

	if (target_id == ISCSI_MAX_TARGET) {
		id = ida_simple_get(&iscsi_sess_ida, 0, 0, GFP_KERNEL);

		if (id < 0) {
			iscsi_cls_session_printk(KERN_ERR, session,
					"Failure in Target ID Allocation\n");
			return id;
		}
		session->target_id = (unsigned int)id;
		session->ida_used = true;
	} else
		session->target_id = target_id;

	dev_set_name(&session->dev, "session%u", session->sid);
	err = device_add(&session->dev);
	if (err) {
		iscsi_cls_session_printk(KERN_ERR, session,
					 "could not register session's dev\n");
		goto release_ida;
	}
	err = transport_register_device(&session->dev);
	if (err) {
		iscsi_cls_session_printk(KERN_ERR, session,
					 "could not register transport's dev\n");
		goto release_dev;
	}

	spin_lock_irqsave(&sesslock, flags);
	list_add(&session->sess_list, &sesslist);
	spin_unlock_irqrestore(&sesslock, flags);

	iscsi_session_event(session, ISCSI_KEVENT_CREATE_SESSION);
	ISCSI_DBG_TRANS_SESSION(session, "Completed session adding\n");
	return 0;

release_dev:
	device_del(&session->dev);
release_ida:
	if (session->ida_used)
		ida_simple_remove(&iscsi_sess_ida, session->target_id);
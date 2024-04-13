	return err;
}

static int iscsi_logout_flashnode_sid(struct iscsi_transport *transport,
				      struct iscsi_uevent *ev)
{
	struct Scsi_Host *shost;
	struct iscsi_cls_session *session;
	int err = 0;

	if (!transport->logout_flashnode_sid) {
		err = -ENOSYS;
		goto exit_logout_sid;
	}

	shost = scsi_host_lookup(ev->u.logout_flashnode_sid.host_no);
	if (!shost) {
		pr_err("%s could not find host no %u\n",
		       __func__, ev->u.logout_flashnode.host_no);
		err = -ENODEV;
		goto put_host;
	}

	session = iscsi_session_lookup(ev->u.logout_flashnode_sid.sid);
	if (!session) {
		pr_err("%s could not find session id %u\n",
		       __func__, ev->u.logout_flashnode_sid.sid);
		err = -EINVAL;
		goto put_host;
	}

	err = transport->logout_flashnode_sid(session);

put_host:
	scsi_host_put(shost);

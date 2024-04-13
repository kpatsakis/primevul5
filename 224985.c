EXPORT_SYMBOL_GPL(iscsi_session_event);

static int
iscsi_if_create_session(struct iscsi_internal *priv, struct iscsi_endpoint *ep,
			struct iscsi_uevent *ev, pid_t pid,
			uint32_t initial_cmdsn,	uint16_t cmds_max,
			uint16_t queue_depth)
{
	struct iscsi_transport *transport = priv->iscsi_transport;
	struct iscsi_cls_session *session;
	struct Scsi_Host *shost;

	session = transport->create_session(ep, cmds_max, queue_depth,
					    initial_cmdsn);
	if (!session)
		return -ENOMEM;

	session->creator = pid;
	shost = iscsi_session_to_shost(session);
	ev->r.c_session_ret.host_no = shost->host_no;
	ev->r.c_session_ret.sid = session->sid;
	ISCSI_DBG_TRANS_SESSION(session,
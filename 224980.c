}
EXPORT_SYMBOL_GPL(iscsi_remove_session);

void iscsi_free_session(struct iscsi_cls_session *session)
{
	ISCSI_DBG_TRANS_SESSION(session, "Freeing session\n");
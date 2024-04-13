	ISCSI_DBG_TRANS_SESSION(session, "Completed target removal\n");
}

static void __iscsi_destroy_session(struct work_struct *work)
{
	struct iscsi_cls_session *session =
		container_of(work, struct iscsi_cls_session, destroy_work);
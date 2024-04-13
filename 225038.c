	return name;
}

int iscsi_session_chkready(struct iscsi_cls_session *session)
{
	int err;

	switch (session->state) {
	case ISCSI_SESSION_LOGGED_IN:
		err = 0;
		break;
	case ISCSI_SESSION_FAILED:
		err = DID_IMM_RETRY << 16;
		break;
	case ISCSI_SESSION_FREE:
		err = DID_TRANSPORT_FAILFAST << 16;
		break;
	default:
		err = DID_NO_CONNECT << 16;
		break;
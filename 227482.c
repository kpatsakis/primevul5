static bool torture_smb2_notify_session_reconnect(struct torture_context *torture,
				struct smb2_tree *tree1)
{
	bool ret = true;
	NTSTATUS status;
	union smb_notify notify;
	union smb_open io;
	struct smb2_handle h1;
	struct smb2_request *req;
	uint64_t previous_session_id = 0;
	struct smb2_session *session2 = NULL;

	smb2_deltree(tree1, BASEDIR_NSR);
	smb2_util_rmdir(tree1, BASEDIR_NSR);

	torture_comment(torture, "TESTING CHANGE NOTIFY FOLLOWED BY SESSION RECONNECT\n");

	/*
	  get a handle on the directory
	*/
	ZERO_STRUCT(io.smb2);
	io.generic.level = RAW_OPEN_SMB2;
	io.smb2.in.create_flags = 0;
	io.smb2.in.desired_access = SEC_FILE_ALL;
	io.smb2.in.create_options = NTCREATEX_OPTIONS_DIRECTORY;
	io.smb2.in.file_attributes = FILE_ATTRIBUTE_NORMAL;
	io.smb2.in.share_access = NTCREATEX_SHARE_ACCESS_READ |
				NTCREATEX_SHARE_ACCESS_WRITE;
	io.smb2.in.alloc_size = 0;
	io.smb2.in.create_disposition = NTCREATEX_DISP_CREATE;
	io.smb2.in.impersonation_level = SMB2_IMPERSONATION_ANONYMOUS;
	io.smb2.in.security_flags = 0;
	io.smb2.in.fname = BASEDIR_NSR;

	status = smb2_create(tree1, torture, &(io.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);

	io.smb2.in.create_disposition = NTCREATEX_DISP_OPEN;
	status = smb2_create(tree1, torture, &(io.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	h1 = io.smb2.out.file.handle;

	/* ask for a change notify,
	   on file or directory name changes */
	ZERO_STRUCT(notify.smb2);
	notify.smb2.level = RAW_NOTIFY_SMB2;
	notify.smb2.in.buffer_size = 1000;
	notify.smb2.in.completion_filter = FILE_NOTIFY_CHANGE_NAME;
	notify.smb2.in.file.handle = h1;
	notify.smb2.in.recursive = true;

	req = smb2_notify_send(tree1, &(notify.smb2));

	WAIT_FOR_ASYNC_RESPONSE(req);

	previous_session_id = smb2cli_session_current_id(tree1->session->smbXcli);
	torture_assert(torture, torture_smb2_session_setup(torture,
		       tree1->session->transport,
		       previous_session_id,
		       torture, &session2),
		       "session setup with previous_session_id failed");

	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, STATUS_NOTIFY_CLEANUP);
	CHECK_VAL(notify.smb2.out.num_changes, 0);

	status = smb2_logoff(tree1->session);
	CHECK_STATUS(status, NT_STATUS_USER_SESSION_DELETED);

	status = smb2_logoff(session2);
	CHECK_STATUS(status, NT_STATUS_OK);
done:
	smb2_deltree(tree1, BASEDIR_NSR);
	return ret;
}
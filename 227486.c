static bool torture_smb2_notify_invalid_reauth(struct torture_context *torture,
					       struct smb2_tree *tree1,
					       struct smb2_tree *tree2)
{
	bool ret = true;
	NTSTATUS status;
	union smb_notify notify;
	union smb_open io;
	struct smb2_handle h1;
	struct smb2_request *req;
	struct cli_credentials *invalid_creds;

	smb2_deltree(tree2, BASEDIR_IR);
	smb2_util_rmdir(tree2, BASEDIR_IR);

	torture_comment(torture, "TESTING CHANGE NOTIFY FOLLOWED BY invalid REAUTH\n");

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
	io.smb2.in.fname = BASEDIR_IR;

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

	invalid_creds = cli_credentials_init(torture);
	torture_assert(torture, (invalid_creds != NULL), "talloc error");
	cli_credentials_set_username(invalid_creds, "__none__invalid__none__", CRED_SPECIFIED);
	cli_credentials_set_domain(invalid_creds, "__none__invalid__none__", CRED_SPECIFIED);
	cli_credentials_set_password(invalid_creds, "__none__invalid__none__", CRED_SPECIFIED);
	cli_credentials_set_realm(invalid_creds, NULL, CRED_SPECIFIED);
	cli_credentials_set_workstation(invalid_creds, "", CRED_UNINITIALISED);

	status = smb2_session_setup_spnego(tree1->session,
					   invalid_creds,
					   0 /* previous_session_id */);
	CHECK_STATUS(status, NT_STATUS_LOGON_FAILURE);

	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, STATUS_NOTIFY_CLEANUP);
	CHECK_VAL(notify.smb2.out.num_changes, 0);

done:
	smb2_deltree(tree2, BASEDIR_IR);
	return ret;
}
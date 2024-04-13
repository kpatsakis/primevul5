static bool torture_smb2_notify_handle_permissions(
		struct torture_context *torture,
		struct smb2_tree *tree)
{
	bool ret = true;
	NTSTATUS status;
	union smb_notify notify;
	union smb_open io;
	struct smb2_handle h1 = {{0}};
	struct smb2_request *req;

	smb2_deltree(tree, BASEDIR_HPERM);
	smb2_util_rmdir(tree, BASEDIR_HPERM);

	torture_comment(torture,
		"TESTING CHANGE NOTIFY "
		"ON A HANDLE WITHOUT PERMISSIONS\n");

	/*
	  get a handle on the directory
	*/
	ZERO_STRUCT(io.smb2);
	io.generic.level = RAW_OPEN_SMB2;
	io.smb2.in.create_flags = 0;
	io.smb2.in.desired_access = SEC_FILE_READ_ATTRIBUTE;
	io.smb2.in.create_options = NTCREATEX_OPTIONS_DIRECTORY;
	io.smb2.in.file_attributes = FILE_ATTRIBUTE_NORMAL;
	io.smb2.in.share_access = NTCREATEX_SHARE_ACCESS_READ |
				NTCREATEX_SHARE_ACCESS_WRITE;
	io.smb2.in.alloc_size = 0;
	io.smb2.in.create_disposition = NTCREATEX_DISP_CREATE;
	io.smb2.in.impersonation_level = SMB2_IMPERSONATION_ANONYMOUS;
	io.smb2.in.security_flags = 0;
	io.smb2.in.fname = BASEDIR_HPERM;

	status = smb2_create(tree, torture, &io.smb2);
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

	req = smb2_notify_send(tree, &notify.smb2);
	torture_assert_goto(torture,
			req != NULL,
			ret,
			done,
			"smb2_notify_send failed\n");

	/*
	 * Cancel it, we don't really want to wait.
	 */
	smb2_cancel(req);
	status = smb2_notify_recv(req, torture, &notify.smb2);
	/* Handle h1 doesn't have permissions for ChangeNotify. */
	CHECK_STATUS(status, NT_STATUS_ACCESS_DENIED);

done:
	if (!smb2_util_handle_empty(h1)) {
		smb2_util_close(tree, h1);
	}
	smb2_deltree(tree, BASEDIR_HPERM);
	return ret;
}
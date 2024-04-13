static bool torture_smb2_notify_rmdir(struct torture_context *torture,
				      struct smb2_tree *tree1,
				      struct smb2_tree *tree2,
				      bool initial_delete_on_close)
{
	bool ret = true;
	NTSTATUS status;
	union smb_notify notify = {};
	union smb_setfileinfo sfinfo = {};
	union smb_open io = {};
	struct smb2_handle h = {};
	struct smb2_request *req;

	torture_comment(torture, "TESTING NOTIFY CANCEL FOR DELETED DIR\n");

	smb2_deltree(tree1, BASEDIR_RMD);
	smb2_util_rmdir(tree1, BASEDIR_RMD);

	ZERO_STRUCT(io.smb2);
	io.generic.level = RAW_OPEN_SMB2;
	io.smb2.in.create_flags = 0;
	io.smb2.in.desired_access = SEC_FILE_ALL;
	io.smb2.in.create_options = NTCREATEX_OPTIONS_DIRECTORY;
	io.smb2.in.file_attributes = FILE_ATTRIBUTE_NORMAL;
	io.smb2.in.share_access =
		NTCREATEX_SHARE_ACCESS_READ |
		NTCREATEX_SHARE_ACCESS_WRITE |
		NTCREATEX_SHARE_ACCESS_DELETE ;
	io.smb2.in.alloc_size = 0;
	io.smb2.in.create_disposition = NTCREATEX_DISP_CREATE;
	io.smb2.in.impersonation_level = SMB2_IMPERSONATION_ANONYMOUS;
	io.smb2.in.security_flags = 0;
	io.smb2.in.fname = BASEDIR_RMD;

	status = smb2_create(tree1, torture, &(io.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	h = io.smb2.out.file.handle;

	ZERO_STRUCT(notify.smb2);
	notify.smb2.level = RAW_NOTIFY_SMB2;
	notify.smb2.in.buffer_size = 1000;
	notify.smb2.in.completion_filter = FILE_NOTIFY_CHANGE_NAME;
	notify.smb2.in.file.handle = h;
	notify.smb2.in.recursive = false;

	io.smb2.in.desired_access |= SEC_STD_DELETE;
	io.smb2.in.create_disposition = NTCREATEX_DISP_OPEN;
	req = smb2_notify_send(tree1, &(notify.smb2));

	if (initial_delete_on_close) {
		status = smb2_util_rmdir(tree2, BASEDIR_RMD);
		CHECK_STATUS(status, NT_STATUS_OK);
	} else {
		status = smb2_create(tree2, torture, &(io.smb2));
		CHECK_STATUS(status, NT_STATUS_OK);

		sfinfo.generic.level = RAW_SFILEINFO_DISPOSITION_INFORMATION;
		sfinfo.generic.in.file.handle = io.smb2.out.file.handle;
		sfinfo.disposition_info.in.delete_on_close = 1;
		status = smb2_setinfo_file(tree2, &sfinfo);
		CHECK_STATUS(status, NT_STATUS_OK);

		smb2_util_close(tree2, io.smb2.out.file.handle);
	}

	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_DELETE_PENDING);

done:

	smb2_util_close(tree1, h);
	smb2_deltree(tree1, BASEDIR_RMD);

	return ret;
}
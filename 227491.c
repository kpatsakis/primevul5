static bool torture_smb2_notify_double(struct torture_context *torture,
			struct smb2_tree *tree1,
			struct smb2_tree *tree2)
{
	bool ret = true;
	NTSTATUS status;
	union smb_notify notify;
	union smb_open io;
	struct smb2_handle h1;
	struct smb2_request *req1, *req2;

	smb2_deltree(tree1, BASEDIR_NDOH);
	smb2_util_rmdir(tree1, BASEDIR_NDOH);

	torture_comment(torture,
		"TESTING CHANGE NOTIFY TWICE ON ONE DIRECTORY\n");

	/*
	  get a handle on the directory
	*/
	ZERO_STRUCT(io.smb2);
	io.generic.level = RAW_OPEN_SMB2;
	io.smb2.in.create_flags = 0;
	io.smb2.in.desired_access = SEC_RIGHTS_FILE_READ|
				SEC_RIGHTS_FILE_WRITE|
				SEC_RIGHTS_FILE_ALL;
	io.smb2.in.create_options = NTCREATEX_OPTIONS_DIRECTORY;
	io.smb2.in.file_attributes = FILE_ATTRIBUTE_NORMAL;
	io.smb2.in.share_access = NTCREATEX_SHARE_ACCESS_READ |
				NTCREATEX_SHARE_ACCESS_WRITE;
	io.smb2.in.alloc_size = 0;
	io.smb2.in.create_disposition = NTCREATEX_DISP_CREATE;
	io.smb2.in.impersonation_level = SMB2_IMPERSONATION_ANONYMOUS;
	io.smb2.in.security_flags = 0;
	io.smb2.in.fname = BASEDIR_NDOH;

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

	req1 = smb2_notify_send(tree1, &(notify.smb2));
	smb2_cancel(req1);
	status = smb2_notify_recv(req1, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_CANCELLED);

	req2 = smb2_notify_send(tree1, &(notify.smb2));
	smb2_cancel(req2);
	status = smb2_notify_recv(req2, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_CANCELLED);

	smb2_util_mkdir(tree2, BASEDIR_NDOH "\\subdir-name");
	req1 = smb2_notify_send(tree1, &(notify.smb2));
	req2 = smb2_notify_send(tree1, &(notify.smb2));

	status = smb2_notify_recv(req1, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VAL(notify.smb2.out.num_changes, 1);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "subdir-name");

	smb2_util_mkdir(tree2, BASEDIR_NDOH "\\subdir-name2");

	status = smb2_notify_recv(req2, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VAL(notify.smb2.out.num_changes, 1);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "subdir-name2");

done:
	smb2_deltree(tree1, BASEDIR_NDOH);
	return ret;
}
static bool torture_smb2_notify_overflow(struct torture_context *torture,
				struct smb2_tree *tree)
{
	bool ret = true;
	NTSTATUS status;
	union smb_notify notify;
	union smb_open io;
	struct smb2_handle h1, h2;
	int count = 100;
	struct smb2_request *req1;
	int i;

	smb2_deltree(tree, BASEDIR_OVF);
	smb2_util_rmdir(tree, BASEDIR_OVF);

	torture_comment(torture, "TESTING CHANGE NOTIFY EVENT OVERFLOW\n");

	/* get a handle on the directory */
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
	io.smb2.in.fname = BASEDIR_OVF;

	status = smb2_create(tree, torture, &(io.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	h1 = io.smb2.out.file.handle;

	/* ask for a change notify, on name changes. */
	ZERO_STRUCT(notify.smb2);
	notify.smb2.level = RAW_NOTIFY_NTTRANS;
	notify.smb2.in.buffer_size = 1000;
	notify.smb2.in.completion_filter = FILE_NOTIFY_CHANGE_NAME;
	notify.smb2.in.file.handle = h1;

	notify.smb2.in.recursive = true;
	req1 = smb2_notify_send(tree, &(notify.smb2));

	/* cancel initial requests so the buffer is setup */
	smb2_cancel(req1);
	status = smb2_notify_recv(req1, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_CANCELLED);

	/* open a lot of files, filling up the server side notify buffer */
	torture_comment(torture,
		"Testing overflowed buffer notify on create of %d files\n",
		count);

	for (i=0;i<count;i++) {
		char *fname = talloc_asprintf(torture,
			      BASEDIR_OVF "\\test%d.txt", i);
		union smb_open io1;
		ZERO_STRUCT(io1.smb2);
	        io1.generic.level = RAW_OPEN_SMB2;
		io1.smb2.in.create_flags = 0;
	        io1.smb2.in.desired_access = SEC_FILE_ALL;
		io1.smb2.in.create_options = NTCREATEX_OPTIONS_DIRECTORY;
		io1.smb2.in.file_attributes = FILE_ATTRIBUTE_NORMAL;
	        io1.smb2.in.share_access = NTCREATEX_SHARE_ACCESS_READ |
				    NTCREATEX_SHARE_ACCESS_WRITE;
	        io1.smb2.in.alloc_size = 0;
	        io1.smb2.in.create_disposition = NTCREATEX_DISP_CREATE;
	        io1.smb2.in.impersonation_level = SMB2_IMPERSONATION_ANONYMOUS;
	        io1.smb2.in.security_flags = 0;
		io1.smb2.in.fname = fname;

		h2 = custom_smb2_create(tree, torture, &(io1.smb2));
		talloc_free(fname);
		smb2_util_close(tree, h2);
	}

	req1 = smb2_notify_send(tree, &(notify.smb2));
	status = smb2_notify_recv(req1, torture, &(notify.smb2));
	CHECK_STATUS(status, STATUS_NOTIFY_ENUM_DIR);
	CHECK_VAL(notify.smb2.out.num_changes, 0);

done:
	smb2_deltree(tree, BASEDIR_OVF);
	return ret;
}
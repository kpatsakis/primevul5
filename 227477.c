static bool torture_smb2_notify_file(struct torture_context *torture,
				struct smb2_tree *tree)
{
	NTSTATUS status;
	bool ret = true;
	union smb_open io;
	union smb_close cl;
	union smb_notify notify;
	struct smb2_request *req;
	struct smb2_handle h1;
	const char *fname = BASEDIR_FL "\\file.txt";

	smb2_deltree(tree, BASEDIR_FL);
	smb2_util_rmdir(tree, BASEDIR_FL);

	torture_comment(torture, "TESTING CHANGE NOTIFY ON FILES\n");
	status = torture_smb2_testdir(tree, BASEDIR_FL, &h1);
	CHECK_STATUS(status, NT_STATUS_OK);

	ZERO_STRUCT(io.smb2);
	io.generic.level = RAW_OPEN_SMB2;
	io.smb2.in.create_flags = 0;
	io.smb2.in.desired_access = SEC_FLAG_MAXIMUM_ALLOWED;
	io.smb2.in.create_options = 0;
	io.smb2.in.file_attributes = FILE_ATTRIBUTE_NORMAL;
	io.smb2.in.share_access = NTCREATEX_SHARE_ACCESS_READ |
				NTCREATEX_SHARE_ACCESS_WRITE;
	io.smb2.in.alloc_size = 0;
	io.smb2.in.create_disposition = NTCREATEX_DISP_CREATE;
	io.smb2.in.impersonation_level = SMB2_IMPERSONATION_ANONYMOUS;
	io.smb2.in.security_flags = 0;
	io.smb2.in.fname = fname;
	status = smb2_create(tree, torture, &(io.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	h1 = io.smb2.out.file.handle;

	/* ask for a change notify,
	   on file or directory name changes */
	ZERO_STRUCT(notify.smb2);
	notify.smb2.level = RAW_NOTIFY_SMB2;
	notify.smb2.in.file.handle = h1;
	notify.smb2.in.buffer_size = 1000;
	notify.smb2.in.completion_filter = FILE_NOTIFY_CHANGE_STREAM_NAME;
	notify.smb2.in.recursive = false;

	torture_comment(torture,
	"Testing if notifies on file handles are invalid (should be)\n");

	req = smb2_notify_send(tree, &(notify.smb2));
	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_INVALID_PARAMETER);

	ZERO_STRUCT(cl.smb2);
	cl.close.level = RAW_CLOSE_SMB2;
	cl.close.in.file.handle = h1;
	status = smb2_close(tree, &(cl.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);

	status = smb2_util_unlink(tree, fname);
	CHECK_STATUS(status, NT_STATUS_OK);

done:
	smb2_deltree(tree, BASEDIR_FL);
	return ret;
}
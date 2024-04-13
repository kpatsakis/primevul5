static bool torture_smb2_notify_basedir(struct torture_context *torture,
				struct smb2_tree *tree1,
				struct smb2_tree *tree2)
{
	bool ret = true;
	NTSTATUS status;
	union smb_notify notify;
	union smb_open io;
	struct smb2_handle h1;
	struct smb2_request *req1;

	smb2_deltree(tree1, BASEDIR_BAS);
	smb2_util_rmdir(tree1, BASEDIR_BAS);

	torture_comment(torture, "TESTING CHANGE NOTIFY BASEDIR EVENTS\n");

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
	io.smb2.in.create_disposition = NTCREATEX_DISP_OPEN_IF;
	io.smb2.in.impersonation_level = NTCREATEX_IMPERSONATION_ANONYMOUS;
	io.smb2.in.security_flags = 0;
	io.smb2.in.fname = BASEDIR_BAS;

	status = smb2_create(tree1, torture, &(io.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	h1 = io.smb2.out.file.handle;

	/* create a test file that will also be modified */
	io.smb2.in.fname = BASEDIR_BAS "\\tname1";
	io.smb2.in.create_options = NTCREATEX_OPTIONS_NON_DIRECTORY_FILE;
	status =  smb2_create(tree2, torture, &(io.smb2));
	CHECK_STATUS(status,NT_STATUS_OK);
	smb2_util_close(tree2, io.smb2.out.file.handle);

	/* ask for a change notify, on attribute changes. */
	ZERO_STRUCT(notify.smb2);
	notify.smb2.level = RAW_NOTIFY_SMB2;
	notify.smb2.in.buffer_size = 1000;
	notify.smb2.in.completion_filter = FILE_NOTIFY_CHANGE_ATTRIBUTES;
	notify.smb2.in.file.handle = h1;
	notify.smb2.in.recursive = true;

	req1 = smb2_notify_send(tree1, &(notify.smb2));

	/* set attribute on the base dir */
	smb2_util_setatr(tree2, BASEDIR_BAS, FILE_ATTRIBUTE_HIDDEN);

	/* set attribute on a file to assure we receive a notification */
	smb2_util_setatr(tree2, BASEDIR_BAS "\\tname1", FILE_ATTRIBUTE_HIDDEN);
	smb_msleep(200);

	/* check how many responses were given, expect only 1 for the file */
	status = smb2_notify_recv(req1, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VAL(notify.smb2.out.num_changes, 1);
	CHECK_VAL(notify.smb2.out.changes[0].action, NOTIFY_ACTION_MODIFIED);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "tname1");

done:
	smb2_deltree(tree1, BASEDIR_BAS);
	return ret;
}
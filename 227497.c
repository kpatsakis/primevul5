static bool torture_smb2_notify_tcon(struct torture_context *torture,
				  struct smb2_tree *tree)
{
	bool ret = true;
	NTSTATUS status;
	union smb_notify notify;
	union smb_open io;
	struct smb2_handle h1 = {{0}};
	struct smb2_request *req = NULL;
	struct smb2_tree *tree1 = NULL;
	const char *fname = BASEDIR_TCON "\\subdir-name";

	smb2_deltree(tree, BASEDIR_TCON);
	smb2_util_rmdir(tree, BASEDIR_TCON);

	torture_comment(torture, "TESTING SIMPLE CHANGE NOTIFY\n");

	/*
	  get a handle on the directory
	*/

	ZERO_STRUCT(io.smb2);
	io.generic.level = RAW_OPEN_SMB2;
	io.smb2.in.create_flags = 0;
	io.smb2.in.desired_access = SEC_RIGHTS_FILE_ALL;
	io.smb2.in.create_options = NTCREATEX_OPTIONS_DIRECTORY;
	io.smb2.in.file_attributes = FILE_ATTRIBUTE_NORMAL |
				FILE_ATTRIBUTE_DIRECTORY;
	io.smb2.in.share_access = NTCREATEX_SHARE_ACCESS_READ |
				NTCREATEX_SHARE_ACCESS_WRITE;
	io.smb2.in.alloc_size = 0;
	io.smb2.in.create_disposition = NTCREATEX_DISP_OPEN_IF;
	io.smb2.in.impersonation_level = SMB2_IMPERSONATION_ANONYMOUS;
	io.smb2.in.security_flags = 0;
	io.smb2.in.fname = BASEDIR_TCON;

	status = smb2_create(tree, torture, &(io.smb2));
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

	torture_comment(torture, "Testing notify mkdir\n");
	req = smb2_notify_send(tree, &(notify.smb2));
	smb2_cancel(req);
	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_CANCELLED);

	notify.smb2.in.recursive = true;
	req = smb2_notify_send(tree, &(notify.smb2));
	status = smb2_util_mkdir(tree, fname);
	CHECK_STATUS(status, NT_STATUS_OK);

	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);

	CHECK_VAL(notify.smb2.out.num_changes, 1);
	CHECK_VAL(notify.smb2.out.changes[0].action, NOTIFY_ACTION_ADDED);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "subdir-name");

	torture_comment(torture, "Testing notify rmdir\n");
	req = smb2_notify_send(tree, &(notify.smb2));
	status = smb2_util_rmdir(tree, fname);
	CHECK_STATUS(status, NT_STATUS_OK);

	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VAL(notify.smb2.out.num_changes, 1);
	CHECK_VAL(notify.smb2.out.changes[0].action, NOTIFY_ACTION_REMOVED);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "subdir-name");

	torture_comment(torture, "SIMPLE CHANGE NOTIFY OK\n");

	torture_comment(torture, "TESTING WITH SECONDARY TCON\n");
	if (!torture_smb2_tree_connect(torture, tree->session, tree, &tree1)) {
		torture_warning(torture, "couldn't reconnect to share, bailing\n");
		ret = false;
		goto done;
	}

	torture_comment(torture, "tid1=%d tid2=%d\n",
			smb2cli_tcon_current_id(tree->smbXcli),
			smb2cli_tcon_current_id(tree1->smbXcli));

	torture_comment(torture, "Testing notify mkdir\n");
	req = smb2_notify_send(tree, &(notify.smb2));
	smb2_util_mkdir(tree1, fname);

	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);

	CHECK_VAL(notify.smb2.out.num_changes, 1);
	CHECK_VAL(notify.smb2.out.changes[0].action, NOTIFY_ACTION_ADDED);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "subdir-name");

	torture_comment(torture, "Testing notify rmdir\n");
	req = smb2_notify_send(tree, &(notify.smb2));
	smb2_util_rmdir(tree, fname);

	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VAL(notify.smb2.out.num_changes, 1);
	CHECK_VAL(notify.smb2.out.changes[0].action, NOTIFY_ACTION_REMOVED);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "subdir-name");

	torture_comment(torture, "CHANGE NOTIFY WITH TCON OK\n");

	torture_comment(torture, "Disconnecting secondary tree\n");
	status = smb2_tdis(tree1);
	CHECK_STATUS(status, NT_STATUS_OK);
	talloc_free(tree1);

	torture_comment(torture, "Testing notify mkdir\n");
	req = smb2_notify_send(tree, &(notify.smb2));
	smb2_util_mkdir(tree, fname);

	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);

	CHECK_VAL(notify.smb2.out.num_changes, 1);
	CHECK_VAL(notify.smb2.out.changes[0].action, NOTIFY_ACTION_ADDED);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "subdir-name");

	torture_comment(torture, "Testing notify rmdir\n");
	req = smb2_notify_send(tree, &(notify.smb2));
	smb2_util_rmdir(tree, fname);

	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VAL(notify.smb2.out.num_changes, 1);
	CHECK_VAL(notify.smb2.out.changes[0].action, NOTIFY_ACTION_REMOVED);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "subdir-name");

	torture_comment(torture, "CHANGE NOTIFY WITH TDIS OK\n");
done:
	smb2_util_close(tree, h1);
	smb2_deltree(tree, BASEDIR_TCON);

	return ret;
}
static bool torture_smb2_notify_recursive(struct torture_context *torture,
				struct smb2_tree *tree1,
				struct smb2_tree *tree2)
{
	bool ret = true;
	NTSTATUS status;
	union smb_notify notify;
	union smb_open io, io1;
	union smb_setfileinfo sinfo;
	struct smb2_handle h1;
	struct smb2_request *req1, *req2;

	smb2_deltree(tree1, BASEDIR_REC);
	smb2_util_rmdir(tree1, BASEDIR_REC);

	torture_comment(torture, "TESTING CHANGE NOTIFY WITH RECURSION\n");

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
	io.smb2.in.fname = BASEDIR_REC;

	status = smb2_create(tree1, torture, &(io.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	h1 = io.smb2.out.file.handle;

	/* ask for a change notify, on file or directory name
	   changes. Setup both with and without recursion */
	ZERO_STRUCT(notify.smb2);
	notify.smb2.level = RAW_NOTIFY_SMB2;
	notify.smb2.in.buffer_size = 1000;
	notify.smb2.in.completion_filter = FILE_NOTIFY_CHANGE_NAME |
				FILE_NOTIFY_CHANGE_ATTRIBUTES |
				FILE_NOTIFY_CHANGE_CREATION;
	notify.smb2.in.file.handle = h1;

	notify.smb2.in.recursive = true;
	req1 = smb2_notify_send(tree1, &(notify.smb2));
	smb2_cancel(req1);
	status = smb2_notify_recv(req1, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_CANCELLED);

	notify.smb2.in.recursive = false;
	req2 = smb2_notify_send(tree1, &(notify.smb2));
	smb2_cancel(req2);
	status = smb2_notify_recv(req2, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_CANCELLED);

	ZERO_STRUCT(io1.smb2);
	io1.generic.level = RAW_OPEN_SMB2;
	io1.smb2.in.create_flags = NTCREATEX_FLAGS_EXTENDED;
	io1.smb2.in.desired_access = SEC_RIGHTS_FILE_READ |
				SEC_RIGHTS_FILE_WRITE|
				SEC_RIGHTS_FILE_ALL;
	io1.smb2.in.create_options = NTCREATEX_OPTIONS_DIRECTORY;
	io1.smb2.in.file_attributes = FILE_ATTRIBUTE_NORMAL;
	io1.smb2.in.share_access = NTCREATEX_SHARE_ACCESS_READ |
				NTCREATEX_SHARE_ACCESS_WRITE |
				NTCREATEX_SHARE_ACCESS_DELETE;
	io1.smb2.in.alloc_size = 0;
	io1.smb2.in.create_disposition = NTCREATEX_DISP_OPEN_IF;
	io1.smb2.in.impersonation_level = SMB2_IMPERSONATION_ANONYMOUS;
	io1.smb2.in.security_flags = 0;
	io1.smb2.in.fname = BASEDIR_REC "\\subdir-name";
	status = smb2_create(tree2, torture, &(io1.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	smb2_util_close(tree2, io1.smb2.out.file.handle);

	io1.smb2.in.fname = BASEDIR_REC "\\subdir-name\\subname1";
	status = smb2_create(tree2, torture, &(io1.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	ZERO_STRUCT(sinfo);
	sinfo.rename_information.level = RAW_SFILEINFO_RENAME_INFORMATION;
	sinfo.rename_information.in.file.handle = io1.smb2.out.file.handle;
	sinfo.rename_information.in.overwrite = 0;
	sinfo.rename_information.in.root_fid = 0;
	sinfo.rename_information.in.new_name =
				BASEDIR_REC "\\subdir-name\\subname1-r";
	status = smb2_setinfo_file(tree2, &sinfo);
	CHECK_STATUS(status, NT_STATUS_OK);

	io1.smb2.in.create_options = NTCREATEX_OPTIONS_NON_DIRECTORY_FILE;
	io1.smb2.in.fname = BASEDIR_REC "\\subdir-name\\subname2";
	status = smb2_create(tree2, torture, &(io1.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	ZERO_STRUCT(sinfo);
	sinfo.rename_information.level = RAW_SFILEINFO_RENAME_INFORMATION;
	sinfo.rename_information.in.file.handle = io1.smb2.out.file.handle;
	sinfo.rename_information.in.overwrite = true;
	sinfo.rename_information.in.root_fid = 0;
	sinfo.rename_information.in.new_name = BASEDIR_REC "\\subname2-r";
	status = smb2_setinfo_file(tree2, &sinfo);
	CHECK_STATUS(status, NT_STATUS_OK);

	io1.smb2.in.fname = BASEDIR_REC "\\subname2-r";
	io1.smb2.in.create_disposition = NTCREATEX_DISP_OPEN;
	status = smb2_create(tree2, torture, &(io1.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	ZERO_STRUCT(sinfo);
	sinfo.rename_information.level = RAW_SFILEINFO_RENAME_INFORMATION;
	sinfo.rename_information.in.file.handle = io1.smb2.out.file.handle;
	sinfo.rename_information.in.overwrite = true;
	sinfo.rename_information.in.root_fid = 0;
	sinfo.rename_information.in.new_name = BASEDIR_REC "\\subname3-r";
	status = smb2_setinfo_file(tree2, &sinfo);
	CHECK_STATUS(status, NT_STATUS_OK);

	notify.smb2.in.completion_filter = 0;
	notify.smb2.in.recursive = true;
	smb_msleep(200);
	req1 = smb2_notify_send(tree1, &(notify.smb2));

	status = smb2_util_rmdir(tree2,
		BASEDIR_REC "\\subdir-name\\subname1-r");
	CHECK_STATUS(status, NT_STATUS_OK);
	status = smb2_util_rmdir(tree2,
		BASEDIR_REC "\\subdir-name");
	CHECK_STATUS(status, NT_STATUS_OK);
	status = smb2_util_unlink(tree2, BASEDIR_REC "\\subname3-r");
	CHECK_STATUS(status, NT_STATUS_OK);

	notify.smb2.in.recursive = false;
	req2 = smb2_notify_send(tree1, &(notify.smb2));

	status = smb2_notify_recv(req1, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);

	CHECK_VAL(notify.smb2.out.num_changes, 9);
	CHECK_VAL(notify.smb2.out.changes[0].action, NOTIFY_ACTION_ADDED);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "subdir-name");
	CHECK_VAL(notify.smb2.out.changes[1].action, NOTIFY_ACTION_ADDED);
	CHECK_WIRE_STR(notify.smb2.out.changes[1].name, "subdir-name\\subname1");
	CHECK_VAL(notify.smb2.out.changes[2].action, NOTIFY_ACTION_OLD_NAME);
	CHECK_WIRE_STR(notify.smb2.out.changes[2].name, "subdir-name\\subname1");
	CHECK_VAL(notify.smb2.out.changes[3].action, NOTIFY_ACTION_NEW_NAME);
	CHECK_WIRE_STR(notify.smb2.out.changes[3].name, "subdir-name\\subname1-r");
	CHECK_VAL(notify.smb2.out.changes[4].action, NOTIFY_ACTION_ADDED);
	CHECK_WIRE_STR(notify.smb2.out.changes[4].name, "subdir-name\\subname2");
	CHECK_VAL(notify.smb2.out.changes[5].action, NOTIFY_ACTION_REMOVED);
	CHECK_WIRE_STR(notify.smb2.out.changes[5].name, "subdir-name\\subname2");
	CHECK_VAL(notify.smb2.out.changes[6].action, NOTIFY_ACTION_ADDED);
	CHECK_WIRE_STR(notify.smb2.out.changes[6].name, "subname2-r");
	CHECK_VAL(notify.smb2.out.changes[7].action, NOTIFY_ACTION_OLD_NAME);
	CHECK_WIRE_STR(notify.smb2.out.changes[7].name, "subname2-r");
	CHECK_VAL(notify.smb2.out.changes[8].action, NOTIFY_ACTION_NEW_NAME);
	CHECK_WIRE_STR(notify.smb2.out.changes[8].name, "subname3-r");

done:
	smb2_deltree(tree1, BASEDIR_REC);
	return ret;
}
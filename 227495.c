static bool torture_smb2_notify_mask_change(struct torture_context *torture,
					    struct smb2_tree *tree1,
					    struct smb2_tree *tree2)
{
	bool ret = true;
	NTSTATUS status;
	union smb_notify notify;
	union smb_open io, io1;
	struct smb2_handle h1;
	struct smb2_request *req1, *req2;
	union smb_setfileinfo sinfo;

	smb2_deltree(tree1, BASEDIR_MC);
	smb2_util_rmdir(tree1, BASEDIR_MC);

	torture_comment(torture, "TESTING CHANGE NOTIFY WITH MASK CHANGE\n");

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
	io.smb2.in.fname = BASEDIR_MC;

	status = smb2_create(tree1, torture, &(io.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	h1 = io.smb2.out.file.handle;

	/* ask for a change notify, on file or directory name
	   changes. Setup both with and without recursion */
	ZERO_STRUCT(notify.smb2);
	notify.smb2.level = RAW_NOTIFY_SMB2;
	notify.smb2.in.buffer_size = 1000;
	notify.smb2.in.completion_filter = FILE_NOTIFY_CHANGE_ATTRIBUTES;
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

	notify.smb2.in.recursive = true;
	req1 = smb2_notify_send(tree1, &(notify.smb2));

	/* Set to hidden then back again. */
	ZERO_STRUCT(io1.smb2);
	io1.generic.level = RAW_OPEN_SMB2;
	io1.smb2.in.create_flags = 0;
	io1.smb2.in.desired_access = SEC_RIGHTS_FILE_READ |
				SEC_RIGHTS_FILE_WRITE|
				SEC_RIGHTS_FILE_ALL;
	io1.smb2.in.file_attributes = FILE_ATTRIBUTE_NORMAL;
	io1.smb2.in.share_access = NTCREATEX_SHARE_ACCESS_READ |
				NTCREATEX_SHARE_ACCESS_WRITE |
				NTCREATEX_SHARE_ACCESS_DELETE;
	io1.smb2.in.impersonation_level = SMB2_IMPERSONATION_ANONYMOUS;
	io1.smb2.in.security_flags = 0;
	io1.smb2.in.create_options = NTCREATEX_OPTIONS_NON_DIRECTORY_FILE;
	io1.smb2.in.create_disposition = NTCREATEX_DISP_CREATE;
	io1.smb2.in.fname = BASEDIR_MC "\\tname1";

	smb2_util_close(tree1,
		custom_smb2_create(tree1, torture, &(io1.smb2)));
	status = smb2_util_setatr(tree1, BASEDIR_MC "\\tname1",
				FILE_ATTRIBUTE_HIDDEN);
	CHECK_STATUS(status, NT_STATUS_OK);
	smb2_util_unlink(tree1, BASEDIR_MC "\\tname1");

	status = smb2_notify_recv(req1, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);

	CHECK_VAL(notify.smb2.out.num_changes, 1);
	CHECK_VAL(notify.smb2.out.changes[0].action, NOTIFY_ACTION_MODIFIED);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "tname1");

	/* Now try and change the mask to include other events.
	 * This should not work - once the mask is set on a directory
	 * h1 it seems to be fixed until the fnum is closed. */

	notify.smb2.in.completion_filter = FILE_NOTIFY_CHANGE_NAME |
					FILE_NOTIFY_CHANGE_ATTRIBUTES |
					FILE_NOTIFY_CHANGE_CREATION;
	notify.smb2.in.recursive = true;
	req1 = smb2_notify_send(tree1, &(notify.smb2));

	notify.smb2.in.recursive = false;
	req2 = smb2_notify_send(tree1, &(notify.smb2));

	io1.smb2.in.create_options = NTCREATEX_OPTIONS_DIRECTORY;
	io1.smb2.in.create_disposition = NTCREATEX_DISP_CREATE;
	io1.smb2.in.fname = BASEDIR_MC "\\subdir-name";
	status = smb2_create(tree2, torture, &(io1.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	smb2_util_close(tree2, io1.smb2.out.file.handle);

	ZERO_STRUCT(sinfo);
	io1.smb2.in.fname = BASEDIR_MC "\\subdir-name\\subname1";
	io1.smb2.in.create_options = NTCREATEX_OPTIONS_DIRECTORY;
	io1.smb2.in.create_disposition = NTCREATEX_DISP_CREATE;
	status = smb2_create(tree2, torture, &(io1.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	sinfo.rename_information.level = RAW_SFILEINFO_RENAME_INFORMATION;
	sinfo.rename_information.in.file.handle = io1.smb2.out.file.handle;
	sinfo.rename_information.in.overwrite = true;
	sinfo.rename_information.in.root_fid = 0;
	sinfo.rename_information.in.new_name =
				BASEDIR_MC "\\subdir-name\\subname1-r";
	status = smb2_setinfo_file(tree2, &sinfo);
	CHECK_STATUS(status, NT_STATUS_OK);

	io1.smb2.in.fname = BASEDIR_MC "\\subdir-name\\subname2";
	io1.smb2.in.create_disposition = NTCREATEX_DISP_CREATE;
	io1.smb2.in.create_options = NTCREATEX_OPTIONS_NON_DIRECTORY_FILE;
	status = smb2_create(tree2, torture, &(io1.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	sinfo.rename_information.in.file.handle = io1.smb2.out.file.handle;
	sinfo.rename_information.in.new_name = BASEDIR_MC "\\subname2-r";
	status = smb2_setinfo_file(tree2, &sinfo);
	CHECK_STATUS(status, NT_STATUS_OK);
	smb2_util_close(tree2, io1.smb2.out.file.handle);

	io1.smb2.in.fname = BASEDIR_MC "\\subname2-r";
	io1.smb2.in.create_disposition = NTCREATEX_DISP_OPEN;
	status = smb2_create(tree2, torture, &(io1.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	sinfo.rename_information.in.file.handle = io1.smb2.out.file.handle;
	sinfo.rename_information.in.new_name = BASEDIR_MC "\\subname3-r";
	status = smb2_setinfo_file(tree2, &sinfo);
	CHECK_STATUS(status, NT_STATUS_OK);
	smb2_util_close(tree2, io1.smb2.out.file.handle);

	status = smb2_util_rmdir(tree2, BASEDIR_MC "\\subdir-name\\subname1-r");
	CHECK_STATUS(status, NT_STATUS_OK);
	status = smb2_util_rmdir(tree2, BASEDIR_MC "\\subdir-name");
	CHECK_STATUS(status, NT_STATUS_OK);
	status = smb2_util_unlink(tree2, BASEDIR_MC "\\subname3-r");
	CHECK_STATUS(status, NT_STATUS_OK);

	status = smb2_notify_recv(req1, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);

	CHECK_VAL(notify.smb2.out.num_changes, 1);
	CHECK_VAL(notify.smb2.out.changes[0].action, NOTIFY_ACTION_MODIFIED);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "subname2-r");

	status = smb2_notify_recv(req2, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);

	CHECK_VAL(notify.smb2.out.num_changes, 1);
	CHECK_VAL(notify.smb2.out.changes[0].action, NOTIFY_ACTION_MODIFIED);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "subname3-r");

	if (!ret) {
		goto done;
	}

done:
	smb2_deltree(tree1, BASEDIR_MC);
	return ret;
}
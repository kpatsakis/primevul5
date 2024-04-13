static bool torture_smb2_notify_dir(struct torture_context *torture,
			      struct smb2_tree *tree1,
			      struct smb2_tree *tree2)
{
	bool ret = true;
	NTSTATUS status;
	union smb_notify notify;
	union smb_open io;
	union smb_close cl;
	int i, count;
	struct smb2_handle h1 = {{0}};
	struct smb2_handle h2 = {{0}};
	struct smb2_request *req, *req2;
	const char *fname = BASEDIR_DIR "\\subdir-name";
	extern int torture_numops;

	torture_comment(torture, "TESTING CHANGE NOTIFY ON DIRECTORIES\n");

	smb2_deltree(tree1, BASEDIR_DIR);
	smb2_util_rmdir(tree1, BASEDIR_DIR);
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
	io.smb2.in.fname = BASEDIR_DIR;

	status = smb2_create(tree1, torture, &(io.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	h1 = io.smb2.out.file.handle;

	io.smb2.in.create_disposition = NTCREATEX_DISP_OPEN;
	io.smb2.in.desired_access = SEC_RIGHTS_FILE_READ;
	status = smb2_create(tree1, torture, &(io.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	h2 = io.smb2.out.file.handle;

	/* ask for a change notify,
	   on file or directory name changes */
	ZERO_STRUCT(notify.smb2);
	notify.smb2.level = RAW_NOTIFY_SMB2;
	notify.smb2.in.buffer_size = 1000;
	notify.smb2.in.completion_filter = FILE_NOTIFY_CHANGE_NAME;
	notify.smb2.in.file.handle = h1;
	notify.smb2.in.recursive = true;

	torture_comment(torture, "Testing notify cancel\n");

	req = smb2_notify_send(tree1, &(notify.smb2));
	smb2_cancel(req);
	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_CANCELLED);

	torture_comment(torture, "Testing notify mkdir\n");

	req = smb2_notify_send(tree1, &(notify.smb2));
	smb2_util_mkdir(tree2, fname);

	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);

	CHECK_VAL(notify.smb2.out.num_changes, 1);
	CHECK_VAL(notify.smb2.out.changes[0].action, NOTIFY_ACTION_ADDED);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "subdir-name");

	torture_comment(torture, "Testing notify rmdir\n");

	req = smb2_notify_send(tree1, &(notify.smb2));
	smb2_util_rmdir(tree2, fname);

	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VAL(notify.smb2.out.num_changes, 1);
	CHECK_VAL(notify.smb2.out.changes[0].action, NOTIFY_ACTION_REMOVED);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "subdir-name");

	torture_comment(torture,
		"Testing notify mkdir - rmdir - mkdir - rmdir\n");

	smb2_util_mkdir(tree2, fname);
	smb2_util_rmdir(tree2, fname);
	smb2_util_mkdir(tree2, fname);
	smb2_util_rmdir(tree2, fname);
	smb_msleep(200);
	req = smb2_notify_send(tree1, &(notify.smb2));
	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VAL(notify.smb2.out.num_changes, 4);
	CHECK_VAL(notify.smb2.out.changes[0].action, NOTIFY_ACTION_ADDED);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "subdir-name");
	CHECK_VAL(notify.smb2.out.changes[1].action, NOTIFY_ACTION_REMOVED);
	CHECK_WIRE_STR(notify.smb2.out.changes[1].name, "subdir-name");
	CHECK_VAL(notify.smb2.out.changes[2].action, NOTIFY_ACTION_ADDED);
	CHECK_WIRE_STR(notify.smb2.out.changes[2].name, "subdir-name");
	CHECK_VAL(notify.smb2.out.changes[3].action, NOTIFY_ACTION_REMOVED);
	CHECK_WIRE_STR(notify.smb2.out.changes[3].name, "subdir-name");

	count = torture_numops;
	torture_comment(torture,
		"Testing buffered notify on create of %d files\n", count);
	for (i=0;i<count;i++) {
		struct smb2_handle h12;
		char *fname2 = talloc_asprintf(torture,
						BASEDIR_DIR "\\test%d.txt",
						i);

		ZERO_STRUCT(io.smb2);
		io.generic.level = RAW_OPEN_SMB2;
	        io.smb2.in.create_flags = 0;
		io.smb2.in.desired_access = SEC_FILE_ALL;
	        io.smb2.in.create_options =
		    NTCREATEX_OPTIONS_NON_DIRECTORY_FILE;
		io.smb2.in.file_attributes = FILE_ATTRIBUTE_NORMAL;
	        io.smb2.in.share_access = NTCREATEX_SHARE_ACCESS_READ |
					NTCREATEX_SHARE_ACCESS_WRITE;
		io.smb2.in.alloc_size = 0;
	        io.smb2.in.create_disposition = NTCREATEX_DISP_CREATE;
		io.smb2.in.impersonation_level = SMB2_IMPERSONATION_ANONYMOUS;
	        io.smb2.in.security_flags = 0;
		io.smb2.in.fname = fname2;

		status = smb2_create(tree1, torture, &(io.smb2));
		if (!NT_STATUS_EQUAL(status, NT_STATUS_OK)) {
			torture_comment(torture, "Failed to create %s \n",
			       fname);
			ret = false;
			goto done;
		}
		h12 = io.smb2.out.file.handle;
		talloc_free(fname2);
		smb2_util_close(tree1, h12);
	}

	/* (1st notify) setup a new notify on a different directory handle.
	   This new notify won't see the events above. */
	notify.smb2.in.file.handle = h2;
	req2 = smb2_notify_send(tree1, &(notify.smb2));

	/* (2nd notify) whereas this notify will see the above buffered events,
	   and it directly returns the buffered events */
	notify.smb2.in.file.handle = h1;
	req = smb2_notify_send(tree1, &(notify.smb2));

	status = smb2_util_unlink(tree1, BASEDIR_DIR "\\nonexistent.txt");
	CHECK_STATUS(status, NT_STATUS_OBJECT_NAME_NOT_FOUND);

	/* (1st unlink) as the 2nd notify directly returns,
	   this unlink is only seen by the 1st notify and
	   the 3rd notify (later) */
	torture_comment(torture,
		"Testing notify on unlink for the first file\n");
	status = smb2_util_unlink(tree2, BASEDIR_DIR "\\test0.txt");
	CHECK_STATUS(status, NT_STATUS_OK);

	/* receive the reply from the 2nd notify */
	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);

	CHECK_VAL(notify.smb2.out.num_changes, count);
	for (i=1;i<count;i++) {
		CHECK_VAL(notify.smb2.out.changes[i].action,
			  NOTIFY_ACTION_ADDED);
	}
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "test0.txt");

	torture_comment(torture, "and now from the 1st notify\n");
	status = smb2_notify_recv(req2, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VAL(notify.smb2.out.num_changes, 1);
	CHECK_VAL(notify.smb2.out.changes[0].action, NOTIFY_ACTION_REMOVED);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "test0.txt");

	torture_comment(torture,
		"(3rd notify) this notify will only see the 1st unlink\n");
	req = smb2_notify_send(tree1, &(notify.smb2));

	status = smb2_util_unlink(tree1, BASEDIR_DIR "\\nonexistent.txt");
	CHECK_STATUS(status, NT_STATUS_OBJECT_NAME_NOT_FOUND);

	for (i=1;i<count;i++) {
		char *fname2 = talloc_asprintf(torture,
			      BASEDIR_DIR "\\test%d.txt", i);
		status = smb2_util_unlink(tree2, fname2);
		CHECK_STATUS(status, NT_STATUS_OK);
		talloc_free(fname2);
	}

	/* receive the 3rd notify */
	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VAL(notify.smb2.out.num_changes, 1);
	CHECK_VAL(notify.smb2.out.changes[0].action, NOTIFY_ACTION_REMOVED);
	CHECK_WIRE_STR(notify.smb2.out.changes[0].name, "test0.txt");

	/* and we now see the rest of the unlink calls on both
	 * directory handles */
	notify.smb2.in.file.handle = h1;
	sleep(3);
	req = smb2_notify_send(tree1, &(notify.smb2));
	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VAL(notify.smb2.out.num_changes, count-1);
	for (i=0;i<notify.smb2.out.num_changes;i++) {
		CHECK_VAL(notify.smb2.out.changes[i].action,
			  NOTIFY_ACTION_REMOVED);
	}
	notify.smb2.in.file.handle = h2;
	req = smb2_notify_send(tree1, &(notify.smb2));
	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VAL(notify.smb2.out.num_changes, count-1);
	for (i=0;i<notify.smb2.out.num_changes;i++) {
		CHECK_VAL(notify.smb2.out.changes[i].action,
			  NOTIFY_ACTION_REMOVED);
	}

	torture_comment(torture,
	"Testing if a close() on the dir handle triggers the notify reply\n");

	notify.smb2.in.file.handle = h1;
	req = smb2_notify_send(tree1, &(notify.smb2));

	ZERO_STRUCT(cl.smb2);
	cl.smb2.level = RAW_CLOSE_SMB2;
	cl.smb2.in.file.handle = h1;
	status = smb2_close(tree1, &(cl.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);

	status = smb2_notify_recv(req, torture, &(notify.smb2));
	CHECK_STATUS(status, STATUS_NOTIFY_CLEANUP);
	CHECK_VAL(notify.smb2.out.num_changes, 9);

done:
	smb2_util_close(tree1, h1);
	smb2_util_close(tree1, h2);
	smb2_deltree(tree1, BASEDIR_DIR);
	return ret;
}
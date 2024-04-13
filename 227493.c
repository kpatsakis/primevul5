static bool torture_smb2_inotify_rename(struct torture_context *torture,
					struct smb2_tree *tree1,
					struct smb2_tree *tree2)
{
	NTSTATUS status;
	struct smb2_notify notify;
	struct notify_changes change1 = {0};
	struct notify_changes change2 = {0};
	struct smb2_create create;
	union smb_setfileinfo sinfo;
	struct smb2_handle h1 = {{0}};
	struct smb2_handle h2 = {{0}};
	struct smb2_request *req;
	struct tevent_timer *te = NULL;
	bool ok = false;

	smb2_deltree(tree1, BASEDIR_INR);

	torture_comment(torture, "Testing change notify of a rename with inotify\n");

	status = torture_smb2_testdir(tree1, BASEDIR_INR, &h1);
	torture_assert_ntstatus_ok_goto(torture, status, ok, done, "torture_smb2_testdir failed");

	ZERO_STRUCT(create);
	create.in.desired_access = SEC_RIGHTS_FILE_READ |
		SEC_RIGHTS_FILE_WRITE|
		SEC_RIGHTS_FILE_ALL;
	create.in.create_options = NTCREATEX_OPTIONS_DIRECTORY;
	create.in.file_attributes = FILE_ATTRIBUTE_NORMAL;
	create.in.share_access = NTCREATEX_SHARE_ACCESS_READ |
		NTCREATEX_SHARE_ACCESS_WRITE |
		NTCREATEX_SHARE_ACCESS_DELETE;
	create.in.create_disposition = NTCREATEX_DISP_OPEN_IF;
	create.in.impersonation_level = SMB2_IMPERSONATION_ANONYMOUS;
	create.in.fname = BASEDIR_INR "\\subdir-name";

	status = smb2_create(tree2, torture, &create);
	torture_assert_ntstatus_ok_goto(torture, status, ok, done, "smb2_create failed\n");
	h2 = create.out.file.handle;

	ZERO_STRUCT(notify);
	notify.level = RAW_NOTIFY_SMB2;
	notify.in.buffer_size = 4096;
	notify.in.completion_filter = FILE_NOTIFY_CHANGE_NAME;
	notify.in.file.handle = h1;
	notify.in.recursive = true;
	req = smb2_notify_send(tree1, &notify);
	torture_assert_not_null_goto(torture, req, ok, done, "smb2_notify_send failed\n");

	while (!NT_STATUS_EQUAL(req->status, STATUS_PENDING)) {
		if (tevent_loop_once(torture->ev) != 0) {
			goto done;
		}
	}

	ZERO_STRUCT(sinfo);
	sinfo.rename_information.level = RAW_SFILEINFO_RENAME_INFORMATION;
	sinfo.rename_information.in.file.handle = h2;
	sinfo.rename_information.in.new_name = BASEDIR_INR "\\subdir-name-r";

	status = smb2_setinfo_file(tree2, &sinfo);
	torture_assert_ntstatus_ok_goto(torture, status, ok, done, "smb2_setinfo_file failed\n");

	smb2_util_close(tree2, h2);

	te = tevent_add_timer(torture->ev,
			      tree1,
			      tevent_timeval_current_ofs(1, 0),
			      notify_timeout,
			      req);
	torture_assert_not_null_goto(torture, te, ok, done, "tevent_add_timer failed\n");

	status = smb2_notify_recv(req, torture, &notify);
	torture_assert_ntstatus_ok_goto(torture, status, ok, done, "smb2_notify_recv failed\n");

	torture_assert_goto(torture, notify.out.num_changes == 1 || notify.out.num_changes == 2,
			    ok, done, "bad notify\n");

	change1 = notify.out.changes[0];
	if (notify.out.num_changes == 2) {
		change2 = notify.out.changes[1];
	} else {
		/*
		 * We may only get one event at a time, so check for the
		 * matching second event for the oldname/newname or
		 * removed/added pair.
		 */
		ZERO_STRUCT(notify);
		notify.level = RAW_NOTIFY_SMB2;
		notify.in.buffer_size = 4096;
		notify.in.completion_filter = FILE_NOTIFY_CHANGE_NAME;
		notify.in.file.handle = h1;
		notify.in.recursive = true;
		req = smb2_notify_send(tree1, &notify);
		torture_assert_not_null_goto(torture, req, ok, done, "smb2_notify_send failed\n");

		status = smb2_notify_recv(req, torture, &notify);
		torture_assert_ntstatus_ok_goto(torture, status, ok, done, "smb2_notify_recv failed\n");

		torture_assert_goto(torture, notify.out.num_changes == 1, ok, done,
				    "bad notify\n");

		change2 = notify.out.changes[0];
	}

	if ((change1.action != NOTIFY_ACTION_OLD_NAME) &&
	    (change1.action != NOTIFY_ACTION_REMOVED))
	{
		torture_fail_goto(torture, done, "bad change notification\n");
	}
	torture_assert_str_equal_goto(torture, change1.name.s, "subdir-name",
			    ok, done, "bad change notification\n");

	if ((change2.action != NOTIFY_ACTION_NEW_NAME) &&
	    (change2.action != NOTIFY_ACTION_ADDED))
	{
		torture_fail_goto(torture, done, "bad change notification\n");
	}
	torture_assert_str_equal_goto(torture, change2.name.s, "subdir-name-r",
			    ok, done, "bad change notification\n");

	ok = true;
done:
	if (!smb2_util_handle_empty(h1)) {
		smb2_util_close(tree1, h1);
	}
	if (!smb2_util_handle_empty(h2)) {
		smb2_util_close(tree2, h2);
	}

	smb2_deltree(tree1, BASEDIR_INR);
	return ok;
}
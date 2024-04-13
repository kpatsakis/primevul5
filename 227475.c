static bool torture_smb2_notify_tree(struct torture_context *torture,
			     struct smb2_tree *tree)
{
	bool ret = true;
	union smb_notify notify;
	union smb_open io;
	struct smb2_request *req;
	struct timeval tv;
	struct {
		const char *path;
		bool recursive;
		uint32_t filter;
		int expected;
		struct smb2_handle h1;
		int counted;
	} dirs[] = {
		{
			.path      = BASEDIR_TREE "\\abc",
			.recursive = true,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 30,
		},
		{
			.path      = BASEDIR_TREE "\\zqy",
			.recursive = true,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 8,
		},
		{
			.path      = BASEDIR_TREE "\\atsy",
			.recursive = true,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 4,
		},
		{
			.path      = BASEDIR_TREE "\\abc\\foo",
			.recursive = true,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 2,
		},
		{
			.path      = BASEDIR_TREE "\\abc\\blah",
			.recursive = true,
			.filter    =  FILE_NOTIFY_CHANGE_NAME,
			.expected  = 13,
		},
		{
			.path      = BASEDIR_TREE "\\abc\\blah",
			.recursive = false,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 7,
		},
		{
			.path      = BASEDIR_TREE "\\abc\\blah\\a",
			.recursive = true,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 2,
		},
		{
			.path      = BASEDIR_TREE "\\abc\\blah\\b",
			.recursive = true,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 2,
		},
		{
			.path      = BASEDIR_TREE "\\abc\\blah\\c",
			.recursive = true,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 2,
		},
		{
			.path      = BASEDIR_TREE "\\abc\\fooblah",
			.recursive = true,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 2,
		},
		{
			.path      = BASEDIR_TREE "\\zqy\\xx",
			.recursive = true,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 2,
		},
		{
			.path      = BASEDIR_TREE "\\zqy\\yyy",
			.recursive = true,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 2,
		},
		{
			.path      = BASEDIR_TREE "\\zqy\\..",
			.recursive = true,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 40,
		},
		{
			.path      = BASEDIR_TREE,
			.recursive = true,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 40,
		},
		{
			.path      = BASEDIR_TREE,
			.recursive = false,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 6,
		},
		{
			.path      = BASEDIR_TREE "\\atsy",
			.recursive = false,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 4,
		},
		{
			.path      = BASEDIR_TREE "\\abc",
			.recursive = true,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 24,
		},
		{
			.path      = BASEDIR_TREE "\\abc",
			.recursive = false,
			.filter    = FILE_NOTIFY_CHANGE_FILE_NAME,
			.expected  = 0,
		},
		{
			.path      = BASEDIR_TREE "\\abc",
			.recursive = true,
			.filter    = FILE_NOTIFY_CHANGE_FILE_NAME,
			.expected  = 0,
		},
		{
			.path      = BASEDIR_TREE "\\abc",
			.recursive = true,
			.filter    = FILE_NOTIFY_CHANGE_NAME,
			.expected  = 24,
		},
	};
	int i;
	NTSTATUS status;
	bool all_done = false;

	smb2_deltree(tree, BASEDIR_TREE);
	smb2_util_rmdir(tree, BASEDIR_TREE);

	torture_comment(torture, "TESTING NOTIFY FOR DIFFERENT DEPTHS\n");

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
	io.smb2.in.impersonation_level = SMB2_IMPERSONATION_ANONYMOUS;
	io.smb2.in.security_flags = 0;
	io.smb2.in.fname = BASEDIR_TREE;
	status = smb2_create(tree, torture, &(io.smb2));
	CHECK_STATUS(status, NT_STATUS_OK);

	ZERO_STRUCT(notify.smb2);
	notify.smb2.level = RAW_NOTIFY_SMB2;
	notify.smb2.in.buffer_size = 20000;

	/*
	  setup the directory tree, and the notify buffer on each directory
	*/
	for (i=0;i<ARRAY_SIZE(dirs);i++) {
		io.smb2.in.fname = dirs[i].path;
		status = smb2_create(tree, torture, &(io.smb2));
		CHECK_STATUS(status, NT_STATUS_OK);
		dirs[i].h1 = io.smb2.out.file.handle;

		notify.smb2.in.completion_filter = dirs[i].filter;
		notify.smb2.in.file.handle = dirs[i].h1;
		notify.smb2.in.recursive = dirs[i].recursive;
		req = smb2_notify_send(tree, &(notify.smb2));
		smb2_cancel(req);
		status = smb2_notify_recv(req, torture, &(notify.smb2));
		CHECK_STATUS(status, NT_STATUS_CANCELLED);
	}

	/* trigger 2 events in each dir */
	for (i=0;i<ARRAY_SIZE(dirs);i++) {
		char *path = talloc_asprintf(torture, "%s\\test.dir",
					     dirs[i].path);
		smb2_util_mkdir(tree, path);
		smb2_util_rmdir(tree, path);
		talloc_free(path);
	}

	/* give a bit of time for the events to propagate */
	tv = timeval_current();

	do {
		/* count events that have happened in each dir */
		for (i=0;i<ARRAY_SIZE(dirs);i++) {
			notify.smb2.in.completion_filter = dirs[i].filter;
			notify.smb2.in.file.handle = dirs[i].h1;
			notify.smb2.in.recursive = dirs[i].recursive;
			req = smb2_notify_send(tree, &(notify.smb2));
			smb2_cancel(req);
			notify.smb2.out.num_changes = 0;
			status = smb2_notify_recv(req, torture,
				 &(notify.smb2));
			dirs[i].counted += notify.smb2.out.num_changes;
		}

		all_done = true;

		for (i=0;i<ARRAY_SIZE(dirs);i++) {
			if (dirs[i].counted != dirs[i].expected) {
				all_done = false;
			}
		}
	} while (!all_done && timeval_elapsed(&tv) < 20);

	torture_comment(torture, "took %.4f seconds to propagate all events\n",
			timeval_elapsed(&tv));

	for (i=0;i<ARRAY_SIZE(dirs);i++) {
		if (dirs[i].counted != dirs[i].expected) {
			torture_comment(torture,
				"ERROR: i=%d expected %d got %d for '%s'\n",
				i, dirs[i].expected, dirs[i].counted,
				dirs[i].path);
			ret = false;
		}
	}

	/*
	  run from the back, closing and deleting
	*/
	for (i=ARRAY_SIZE(dirs)-1;i>=0;i--) {
		smb2_util_close(tree, dirs[i].h1);
		smb2_util_rmdir(tree, dirs[i].path);
	}

done:
	smb2_deltree(tree, BASEDIR_TREE);
	smb2_util_rmdir(tree, BASEDIR_TREE);
	return ret;
}
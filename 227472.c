static bool torture_smb2_notify_mask(struct torture_context *torture,
				     struct smb2_tree *tree1,
				     struct smb2_tree *tree2)
{
	bool ret = true;
	NTSTATUS status;
	union smb_notify notify;
	union smb_open io, io1;
	struct smb2_handle h1, h2;
	uint32_t mask;
	int i;
	char c = 1;
	union smb_setfileinfo sinfo;

	smb2_deltree(tree1, BASEDIR_MSK);
	smb2_util_rmdir(tree1, BASEDIR_MSK);

	torture_comment(torture, "TESTING CHANGE NOTIFY COMPLETION FILTERS\n");


	ZERO_STRUCT(h1);
	ZERO_STRUCT(h2);
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
	io.smb2.in.create_disposition = NTCREATEX_DISP_OPEN_IF;
	io.smb2.in.impersonation_level = SMB2_IMPERSONATION_ANONYMOUS;
	io.smb2.in.security_flags = 0;
	io.smb2.in.fname = BASEDIR_MSK;

	ZERO_STRUCT(notify.smb2);
	notify.smb2.level = RAW_NOTIFY_SMB2;
	notify.smb2.in.buffer_size = 1000;
	notify.smb2.in.recursive = true;

#define NOTIFY_MASK_TEST(test_name, setup, op, cleanup, Action, \
			 expected, nchanges) \
	do { \
	do { for (mask=i=0;i<32;i++) { \
		struct smb2_request *req; \
		status = smb2_create(tree1, torture, &(io.smb2)); \
		CHECK_STATUS(status, NT_STATUS_OK); \
		h1 = io.smb2.out.file.handle; \
		setup \
		notify.smb2.in.file.handle = h1;	\
		notify.smb2.in.completion_filter = ((uint32_t)1<<i); \
		/* cancel initial requests so the buffer is setup */	\
		req = smb2_notify_send(tree1, &(notify.smb2)); \
		smb2_cancel(req); \
		status = smb2_notify_recv(req, torture, &(notify.smb2)); \
		CHECK_STATUS(status, NT_STATUS_CANCELLED); \
		/* send the change notify request */ \
		req = smb2_notify_send(tree1, &(notify.smb2)); \
		op \
		smb_msleep(200); smb2_cancel(req); \
		status = smb2_notify_recv(req, torture, &(notify.smb2)); \
		cleanup \
		smb2_util_close(tree1, h1); \
		if (NT_STATUS_EQUAL(status, NT_STATUS_CANCELLED)) continue; \
		CHECK_STATUS(status, NT_STATUS_OK); \
		/* special case to cope with file rename behaviour */ \
		if (nchanges == 2 && notify.smb2.out.num_changes == 1 && \
		    notify.smb2.out.changes[0].action == \
			NOTIFY_ACTION_MODIFIED && \
		    ((expected) & FILE_NOTIFY_CHANGE_ATTRIBUTES) && \
		    Action == NOTIFY_ACTION_OLD_NAME) { \
			torture_comment(torture, \
				"(rename file special handling OK)\n"); \
		} else if (nchanges != notify.smb2.out.num_changes) { \
			torture_result(torture, TORTURE_FAIL, \
			       "ERROR: nchanges=%d expected=%d "\
			       "action=%d filter=0x%08x\n", \
			       notify.smb2.out.num_changes, \
			       nchanges, \
			       notify.smb2.out.changes[0].action, \
			       notify.smb2.in.completion_filter); \
			ret = false; \
		} else if (notify.smb2.out.changes[0].action != Action) { \
			torture_result(torture, TORTURE_FAIL, \
			       "ERROR: nchanges=%d action=%d " \
			       "expectedAction=%d filter=0x%08x\n", \
			       notify.smb2.out.num_changes, \
			       notify.smb2.out.changes[0].action, \
			       Action, \
			       notify.smb2.in.completion_filter); \
			ret = false; \
		} else if (strcmp(notify.smb2.out.changes[0].name.s, \
			   "tname1") != 0) { \
			torture_result(torture, TORTURE_FAIL, \
			       "ERROR: nchanges=%d action=%d " \
			       "filter=0x%08x name=%s\n", \
			       notify.smb2.out.num_changes, \
			       notify.smb2.out.changes[0].action, \
			       notify.smb2.in.completion_filter, \
			       notify.smb2.out.changes[0].name.s);	\
			ret = false; \
		} \
		mask |= ((uint32_t)1<<i); \
	} \
	} while (0); \
	} while (0);

	torture_comment(torture, "Testing mkdir\n");
	NOTIFY_MASK_TEST("Testing mkdir",;,
			 smb2_util_mkdir(tree2, BASEDIR_MSK "\\tname1");,
			 smb2_util_rmdir(tree2, BASEDIR_MSK "\\tname1");,
			 NOTIFY_ACTION_ADDED,
			 FILE_NOTIFY_CHANGE_DIR_NAME, 1);

	torture_comment(torture, "Testing create file\n");
	ZERO_STRUCT(io1.smb2);
	io1.generic.level = RAW_OPEN_SMB2;
	io1.smb2.in.create_flags = 0;
	io1.smb2.in.desired_access = SEC_FILE_ALL;
	io1.smb2.in.file_attributes = FILE_ATTRIBUTE_NORMAL;
	io1.smb2.in.share_access = NTCREATEX_SHARE_ACCESS_READ |
				NTCREATEX_SHARE_ACCESS_WRITE;
	io1.smb2.in.impersonation_level = SMB2_IMPERSONATION_ANONYMOUS;
	io1.smb2.in.security_flags = 0;
	io1.smb2.in.create_options = NTCREATEX_OPTIONS_NON_DIRECTORY_FILE;
	io1.smb2.in.create_disposition = NTCREATEX_DISP_CREATE;
	io1.smb2.in.fname = BASEDIR_MSK "\\tname1";

	NOTIFY_MASK_TEST("Testing create file",;,
			 smb2_util_close(tree2, custom_smb2_create(tree2,
						torture, &(io1.smb2)));,
			 smb2_util_unlink(tree2, BASEDIR_MSK "\\tname1");,
			 NOTIFY_ACTION_ADDED,
			 FILE_NOTIFY_CHANGE_FILE_NAME, 1);

	torture_comment(torture, "Testing unlink\n");
	NOTIFY_MASK_TEST("Testing unlink",
			 smb2_util_close(tree2, custom_smb2_create(tree2,
						torture, &(io1.smb2)));,
			 smb2_util_unlink(tree2, BASEDIR_MSK "\\tname1");,
			 ;,
			 NOTIFY_ACTION_REMOVED,
			 FILE_NOTIFY_CHANGE_FILE_NAME, 1);

	torture_comment(torture, "Testing rmdir\n");
	NOTIFY_MASK_TEST("Testing rmdir",
			 smb2_util_mkdir(tree2, BASEDIR_MSK "\\tname1");,
			 smb2_util_rmdir(tree2, BASEDIR_MSK "\\tname1");,
			 ;,
			 NOTIFY_ACTION_REMOVED,
			 FILE_NOTIFY_CHANGE_DIR_NAME, 1);

	torture_comment(torture, "Testing rename file\n");
	ZERO_STRUCT(sinfo);
	sinfo.rename_information.level = RAW_SFILEINFO_RENAME_INFORMATION;
	sinfo.rename_information.in.file.handle = h1;
	sinfo.rename_information.in.overwrite = true;
	sinfo.rename_information.in.root_fid = 0;
	sinfo.rename_information.in.new_name = BASEDIR_MSK "\\tname2";
	NOTIFY_MASK_TEST("Testing rename file",
			 smb2_util_close(tree2, custom_smb2_create(tree2,
						torture, &(io1.smb2)));,
			 smb2_setinfo_file(tree2, &sinfo);,
			 smb2_util_unlink(tree2, BASEDIR_MSK "\\tname2");,
			 NOTIFY_ACTION_OLD_NAME,
			 FILE_NOTIFY_CHANGE_FILE_NAME, 2);

	torture_comment(torture, "Testing rename dir\n");
	ZERO_STRUCT(sinfo);
	sinfo.rename_information.level = RAW_SFILEINFO_RENAME_INFORMATION;
	sinfo.rename_information.in.file.handle = h1;
	sinfo.rename_information.in.overwrite = true;
	sinfo.rename_information.in.root_fid = 0;
	sinfo.rename_information.in.new_name = BASEDIR_MSK "\\tname2";
	NOTIFY_MASK_TEST("Testing rename dir",
		smb2_util_mkdir(tree2, BASEDIR_MSK "\\tname1");,
		smb2_setinfo_file(tree2, &sinfo);,
		smb2_util_rmdir(tree2, BASEDIR_MSK "\\tname2");,
		NOTIFY_ACTION_OLD_NAME,
		FILE_NOTIFY_CHANGE_DIR_NAME, 2);

	torture_comment(torture, "Testing set path attribute\n");
	NOTIFY_MASK_TEST("Testing set path attribute",
		smb2_util_close(tree2, custom_smb2_create(tree2,
				       torture, &(io.smb2)));,
		smb2_util_setatr(tree2, BASEDIR_MSK "\\tname1",
				 FILE_ATTRIBUTE_HIDDEN);,
		smb2_util_unlink(tree2, BASEDIR_MSK "\\tname1");,
		NOTIFY_ACTION_MODIFIED,
		FILE_NOTIFY_CHANGE_ATTRIBUTES, 1);

	torture_comment(torture, "Testing set path write time\n");
	ZERO_STRUCT(sinfo);
	sinfo.generic.level = RAW_SFILEINFO_BASIC_INFORMATION;
	sinfo.generic.in.file.handle = h1;
	sinfo.basic_info.in.write_time = 1000;
	NOTIFY_MASK_TEST("Testing set path write time",
		smb2_util_close(tree2, custom_smb2_create(tree2,
				       torture, &(io1.smb2)));,
		smb2_setinfo_file(tree2, &sinfo);,
		smb2_util_unlink(tree2, BASEDIR_MSK "\\tname1");,
		NOTIFY_ACTION_MODIFIED,
		FILE_NOTIFY_CHANGE_LAST_WRITE, 1);

	if (torture_setting_bool(torture, "samba3", false)) {
		torture_comment(torture,
		       "Samba3 does not yet support create times "
		       "everywhere\n");
	}
	else {
		ZERO_STRUCT(sinfo);
	        sinfo.generic.level = RAW_SFILEINFO_BASIC_INFORMATION;
		sinfo.generic.in.file.handle = h1;
	        sinfo.basic_info.in.create_time = 0;
		torture_comment(torture, "Testing set file create time\n");
		NOTIFY_MASK_TEST("Testing set file create time",
			smb2_create_complex_file(torture, tree2,
			BASEDIR_MSK "\\tname1", &h2);,
			smb2_setinfo_file(tree2, &sinfo);,
			(smb2_util_close(tree2, h2),
			 smb2_util_unlink(tree2, BASEDIR_MSK "\\tname1"));,
			NOTIFY_ACTION_MODIFIED,
			FILE_NOTIFY_CHANGE_CREATION, 1);
	}

	ZERO_STRUCT(sinfo);
	sinfo.generic.level = RAW_SFILEINFO_BASIC_INFORMATION;
	sinfo.generic.in.file.handle = h1;
	sinfo.basic_info.in.access_time = 0;
	torture_comment(torture, "Testing set file access time\n");
	NOTIFY_MASK_TEST("Testing set file access time",
		smb2_create_complex_file(torture,
			tree2,
			BASEDIR_MSK "\\tname1",
			&h2);,
		smb2_setinfo_file(tree2, &sinfo);,
		(smb2_util_close(tree2, h2),
		smb2_util_unlink(tree2, BASEDIR_MSK "\\tname1"));,
		NOTIFY_ACTION_MODIFIED,
		FILE_NOTIFY_CHANGE_LAST_ACCESS, 1);

	ZERO_STRUCT(sinfo);
	sinfo.generic.level = RAW_SFILEINFO_BASIC_INFORMATION;
	sinfo.generic.in.file.handle = h1;
	sinfo.basic_info.in.change_time = 0;
	torture_comment(torture, "Testing set file change time\n");
	NOTIFY_MASK_TEST("Testing set file change time",
		smb2_create_complex_file(torture,
			tree2,
			BASEDIR_MSK "\\tname1",
			&h2);,
		smb2_setinfo_file(tree2, &sinfo);,
		(smb2_util_close(tree2, h2),
		smb2_util_unlink(tree2, BASEDIR_MSK "\\tname1"));,
		NOTIFY_ACTION_MODIFIED,
		0, 1);


	torture_comment(torture, "Testing write\n");
	NOTIFY_MASK_TEST("Testing write",
		smb2_create_complex_file(torture,
			tree2,
			BASEDIR_MSK "\\tname1",
			&h2);,
		smb2_util_write(tree2, h2, &c, 10000, 1);,
		(smb2_util_close(tree2, h2),
		smb2_util_unlink(tree2, BASEDIR_MSK "\\tname1"));,
		NOTIFY_ACTION_MODIFIED,
		0, 1);

done:
	smb2_deltree(tree1, BASEDIR_MSK);
	return ret;
}
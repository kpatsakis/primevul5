struct torture_suite *torture_smb2_notify_inotify_init(TALLOC_CTX *ctx)
{
	struct torture_suite *suite = torture_suite_create(ctx, "notify-inotify");

	suite->description = talloc_strdup(suite, "SMB2-NOTIFY tests that use inotify");

	torture_suite_add_2smb2_test(suite, "inotify-rename", torture_smb2_inotify_rename);

	return suite;
}
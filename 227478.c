static bool torture_smb2_notify_rmdir1(struct torture_context *torture,
				       struct smb2_tree *tree)
{
	return torture_smb2_notify_rmdir(torture, tree, tree, false);
}
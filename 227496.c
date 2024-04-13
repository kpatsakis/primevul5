static bool torture_smb2_notify_rmdir4(struct torture_context *torture,
				       struct smb2_tree *tree1,
				       struct smb2_tree *tree2)
{
	return torture_smb2_notify_rmdir(torture, tree1, tree2, true);
}
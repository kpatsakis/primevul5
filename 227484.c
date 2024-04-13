static struct smb2_handle custom_smb2_create(struct smb2_tree *tree,
						struct torture_context *torture,
						struct smb2_create *smb2)
{
	struct smb2_handle h1;
	bool ret = true;
	NTSTATUS status;
	smb2_deltree(tree, smb2->in.fname);
	status = smb2_create(tree, torture, smb2);
	CHECK_STATUS(status, NT_STATUS_OK);
	h1 = smb2->out.file.handle;
done:
	if (!ret) {
		h1 = (struct smb2_handle) {
			.data = { 0 , 0},
		};
	}
	return h1;
}
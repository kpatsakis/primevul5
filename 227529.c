static void kvm_create_vcpu_debugfs(struct kvm_vcpu *vcpu)
{
#ifdef __KVM_HAVE_ARCH_VCPU_DEBUGFS
	struct dentry *debugfs_dentry;
	char dir_name[ITOA_MAX_LEN * 2];

	if (!debugfs_initialized())
		return;

	snprintf(dir_name, sizeof(dir_name), "vcpu%d", vcpu->vcpu_id);
	debugfs_dentry = debugfs_create_dir(dir_name,
					    vcpu->kvm->debugfs_dentry);

	kvm_arch_create_vcpu_debugfs(vcpu, debugfs_dentry);
#endif
}
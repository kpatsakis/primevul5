static int kvm_vcpu_release(struct inode *inode, struct file *filp)
{
	struct kvm_vcpu *vcpu = filp->private_data;

	debugfs_remove_recursive(vcpu->debugfs_dentry);
	kvm_put_kvm(vcpu->kvm);
	return 0;
}
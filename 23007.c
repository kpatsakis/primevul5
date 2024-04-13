int kvm_vm_ioctl_rtas_define_token(struct kvm *kvm, void __user *argp)
{
	struct kvm_rtas_token_args args;
	int rc;

	if (copy_from_user(&args, argp, sizeof(args)))
		return -EFAULT;

	mutex_lock(&kvm->arch.rtas_token_lock);

	if (args.token)
		rc = rtas_token_define(kvm, args.name, args.token);
	else
		rc = rtas_token_undefine(kvm, args.name);

	mutex_unlock(&kvm->arch.rtas_token_lock);

	return rc;
}
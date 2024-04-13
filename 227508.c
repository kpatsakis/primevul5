bool file_is_kvm(struct file *file)
{
	return file && file->f_op == &kvm_vm_fops;
}
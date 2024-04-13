void kvm_arch_exit(void)
{
	kvm_s390_gib_destroy();
	debug_unregister(kvm_s390_dbf);
	debug_unregister(kvm_s390_dbf_uv);
}
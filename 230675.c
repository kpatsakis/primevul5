static int get_npt_level(struct kvm_vcpu *vcpu)
{
#ifdef CONFIG_X86_64
	return PT64_ROOT_4LEVEL;
#else
	return PT32E_ROOT_LEVEL;
#endif
}
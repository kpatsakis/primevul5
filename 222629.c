static void check_processor_compat(void *rtn)
{
	*(int *)rtn = kvm_arch_check_processor_compat();
}
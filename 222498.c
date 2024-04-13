int kvm_arch_hardware_enable(void)
{
	/* every s390 is virtualization enabled ;-) */
	return 0;
}
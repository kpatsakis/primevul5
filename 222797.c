static void allow_cpu_feat(unsigned long nr)
{
	set_bit_inv(nr, kvm_s390_available_cpu_feat);
}
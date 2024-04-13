static bool kvm_has_pckmo_subfunc(struct kvm *kvm, unsigned long nr)
{
	if (test_bit_inv(nr, (unsigned long *)&kvm->arch.model.subfuncs.pckmo) &&
	    test_bit_inv(nr, (unsigned long *)&kvm_s390_available_subfunc.pckmo))
		return true;
	return false;
}
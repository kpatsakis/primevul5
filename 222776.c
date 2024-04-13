static bool kvm_has_pckmo_ecc(struct kvm *kvm)
{
	/* At least one ECC subfunction must be present */
	return kvm_has_pckmo_subfunc(kvm, 32) ||
	       kvm_has_pckmo_subfunc(kvm, 33) ||
	       kvm_has_pckmo_subfunc(kvm, 34) ||
	       kvm_has_pckmo_subfunc(kvm, 40) ||
	       kvm_has_pckmo_subfunc(kvm, 41);

}
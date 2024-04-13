void kvm_arch_destroy_vm(struct kvm *kvm)
{
	u16 rc, rrc;

	kvm_free_vcpus(kvm);
	sca_dispose(kvm);
	kvm_s390_gisa_destroy(kvm);
	/*
	 * We are already at the end of life and kvm->lock is not taken.
	 * This is ok as the file descriptor is closed by now and nobody
	 * can mess with the pv state. To avoid lockdep_assert_held from
	 * complaining we do not use kvm_s390_pv_is_protected.
	 */
	if (kvm_s390_pv_get_handle(kvm))
		kvm_s390_pv_deinit_vm(kvm, &rc, &rrc);
	debug_unregister(kvm->arch.dbf);
	free_page((unsigned long)kvm->arch.sie_page2);
	if (!kvm_is_ucontrol(kvm))
		gmap_remove(kvm->arch.gmap);
	kvm_s390_destroy_adapters(kvm);
	kvm_s390_clear_float_irqs(kvm);
	kvm_s390_vsie_destroy(kvm);
	KVM_EVENT(3, "vm 0x%pK destroyed", kvm);
}
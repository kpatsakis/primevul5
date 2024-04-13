void kvm_arch_vcpu_destroy(struct kvm_vcpu *vcpu)
{
	u16 rc, rrc;

	VCPU_EVENT(vcpu, 3, "%s", "free cpu");
	trace_kvm_s390_destroy_vcpu(vcpu->vcpu_id);
	kvm_s390_clear_local_irqs(vcpu);
	kvm_clear_async_pf_completion_queue(vcpu);
	if (!kvm_is_ucontrol(vcpu->kvm))
		sca_del_vcpu(vcpu);

	if (kvm_is_ucontrol(vcpu->kvm))
		gmap_remove(vcpu->arch.gmap);

	if (vcpu->kvm->arch.use_cmma)
		kvm_s390_vcpu_unsetup_cmma(vcpu);
	/* We can not hold the vcpu mutex here, we are already dying */
	if (kvm_s390_pv_cpu_get_handle(vcpu))
		kvm_s390_pv_destroy_cpu(vcpu, &rc, &rrc);
	free_page((unsigned long)(vcpu->arch.sie_block));
}
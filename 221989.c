static void kvm_send_ipi_to_many(struct kvm *kvm, u32 vector,
				 unsigned long *vcpu_bitmap)
{
	struct kvm_lapic_irq irq = {
		.delivery_mode = APIC_DM_FIXED,
		.vector = vector
	};
	struct kvm_vcpu *vcpu;
	int i;

	kvm_for_each_vcpu(i, vcpu, kvm) {
		if (vcpu_bitmap && !test_bit(i, vcpu_bitmap))
			continue;

		/* We fail only when APIC is disabled */
		kvm_apic_set_irq(vcpu, &irq, NULL);
	}
}
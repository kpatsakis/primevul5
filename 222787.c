static int kvm_clock_sync(struct notifier_block *notifier, unsigned long val,
			  void *v)
{
	struct kvm *kvm;
	struct kvm_vcpu *vcpu;
	int i;
	unsigned long long *delta = v;

	list_for_each_entry(kvm, &vm_list, vm_list) {
		kvm_for_each_vcpu(i, vcpu, kvm) {
			kvm_clock_sync_scb(vcpu->arch.sie_block, *delta);
			if (i == 0) {
				kvm->arch.epoch = vcpu->arch.sie_block->epoch;
				kvm->arch.epdx = vcpu->arch.sie_block->epdx;
			}
			if (vcpu->arch.cputm_enabled)
				vcpu->arch.cputm_start += *delta;
			if (vcpu->arch.vsie_block)
				kvm_clock_sync_scb(vcpu->arch.vsie_block,
						   *delta);
		}
	}
	return NOTIFY_OK;
}
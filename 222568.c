void kvm_s390_set_tod_clock(struct kvm *kvm,
			    const struct kvm_s390_vm_tod_clock *gtod)
{
	struct kvm_vcpu *vcpu;
	struct kvm_s390_tod_clock_ext htod;
	int i;

	mutex_lock(&kvm->lock);
	preempt_disable();

	get_tod_clock_ext((char *)&htod);

	kvm->arch.epoch = gtod->tod - htod.tod;
	kvm->arch.epdx = 0;
	if (test_kvm_facility(kvm, 139)) {
		kvm->arch.epdx = gtod->epoch_idx - htod.epoch_idx;
		if (kvm->arch.epoch > gtod->tod)
			kvm->arch.epdx -= 1;
	}

	kvm_s390_vcpu_block_all(kvm);
	kvm_for_each_vcpu(i, vcpu, kvm) {
		vcpu->arch.sie_block->epoch = kvm->arch.epoch;
		vcpu->arch.sie_block->epdx  = kvm->arch.epdx;
	}

	kvm_s390_vcpu_unblock_all(kvm);
	preempt_enable();
	mutex_unlock(&kvm->lock);
}
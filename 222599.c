static void kvm_s390_get_tod_clock(struct kvm *kvm,
				   struct kvm_s390_vm_tod_clock *gtod)
{
	struct kvm_s390_tod_clock_ext htod;

	preempt_disable();

	get_tod_clock_ext((char *)&htod);

	gtod->tod = htod.tod + kvm->arch.epoch;
	gtod->epoch_idx = 0;
	if (test_kvm_facility(kvm, 139)) {
		gtod->epoch_idx = htod.epoch_idx + kvm->arch.epdx;
		if (gtod->tod < htod.tod)
			gtod->epoch_idx += 1;
	}

	preempt_enable();
}
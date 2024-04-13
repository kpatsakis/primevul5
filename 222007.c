static int kvm_hv_vcpu_init(struct kvm_vcpu *vcpu)
{
	struct kvm_vcpu_hv *hv_vcpu;
	int i;

	hv_vcpu = kzalloc(sizeof(struct kvm_vcpu_hv), GFP_KERNEL_ACCOUNT);
	if (!hv_vcpu)
		return -ENOMEM;

	vcpu->arch.hyperv = hv_vcpu;
	hv_vcpu->vcpu = vcpu;

	synic_init(&hv_vcpu->synic);

	bitmap_zero(hv_vcpu->stimer_pending_bitmap, HV_SYNIC_STIMER_COUNT);
	for (i = 0; i < ARRAY_SIZE(hv_vcpu->stimer); i++)
		stimer_init(&hv_vcpu->stimer[i], i);

	hv_vcpu->vp_index = kvm_vcpu_get_idx(vcpu);

	return 0;
}
int kvm_arch_vcpu_create(struct kvm_vcpu *vcpu)
{
	struct sie_page *sie_page;
	int rc;

	BUILD_BUG_ON(sizeof(struct sie_page) != 4096);
	sie_page = (struct sie_page *) get_zeroed_page(GFP_KERNEL);
	if (!sie_page)
		return -ENOMEM;

	vcpu->arch.sie_block = &sie_page->sie_block;
	vcpu->arch.sie_block->itdba = (unsigned long) &sie_page->itdb;

	/* the real guest size will always be smaller than msl */
	vcpu->arch.sie_block->mso = 0;
	vcpu->arch.sie_block->msl = sclp.hamax;

	vcpu->arch.sie_block->icpua = vcpu->vcpu_id;
	spin_lock_init(&vcpu->arch.local_int.lock);
	vcpu->arch.sie_block->gd = (u32)(u64)vcpu->kvm->arch.gisa_int.origin;
	if (vcpu->arch.sie_block->gd && sclp.has_gisaf)
		vcpu->arch.sie_block->gd |= GISA_FORMAT1;
	seqcount_init(&vcpu->arch.cputm_seqcount);

	vcpu->arch.pfault_token = KVM_S390_PFAULT_TOKEN_INVALID;
	kvm_clear_async_pf_completion_queue(vcpu);
	vcpu->run->kvm_valid_regs = KVM_SYNC_PREFIX |
				    KVM_SYNC_GPRS |
				    KVM_SYNC_ACRS |
				    KVM_SYNC_CRS |
				    KVM_SYNC_ARCH0 |
				    KVM_SYNC_PFAULT;
	kvm_s390_set_prefix(vcpu, 0);
	if (test_kvm_facility(vcpu->kvm, 64))
		vcpu->run->kvm_valid_regs |= KVM_SYNC_RICCB;
	if (test_kvm_facility(vcpu->kvm, 82))
		vcpu->run->kvm_valid_regs |= KVM_SYNC_BPBC;
	if (test_kvm_facility(vcpu->kvm, 133))
		vcpu->run->kvm_valid_regs |= KVM_SYNC_GSCB;
	if (test_kvm_facility(vcpu->kvm, 156))
		vcpu->run->kvm_valid_regs |= KVM_SYNC_ETOKEN;
	/* fprs can be synchronized via vrs, even if the guest has no vx. With
	 * MACHINE_HAS_VX, (load|store)_fpu_regs() will work with vrs format.
	 */
	if (MACHINE_HAS_VX)
		vcpu->run->kvm_valid_regs |= KVM_SYNC_VRS;
	else
		vcpu->run->kvm_valid_regs |= KVM_SYNC_FPRS;

	if (kvm_is_ucontrol(vcpu->kvm)) {
		rc = __kvm_ucontrol_vcpu_init(vcpu);
		if (rc)
			goto out_free_sie_block;
	}

	VM_EVENT(vcpu->kvm, 3, "create cpu %d at 0x%pK, sie block at 0x%pK",
		 vcpu->vcpu_id, vcpu, vcpu->arch.sie_block);
	trace_kvm_s390_create_vcpu(vcpu->vcpu_id, vcpu, vcpu->arch.sie_block);

	rc = kvm_s390_vcpu_setup(vcpu);
	if (rc)
		goto out_ucontrol_uninit;
	return 0;

out_ucontrol_uninit:
	if (kvm_is_ucontrol(vcpu->kvm))
		gmap_remove(vcpu->arch.gmap);
out_free_sie_block:
	free_page((unsigned long)(vcpu->arch.sie_block));
	return rc;
}
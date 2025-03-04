static void nested_svm_init_mmu_context(struct kvm_vcpu *vcpu)
{
	kvm_init_shadow_mmu(vcpu, &vcpu->arch.mmu);

	vcpu->arch.mmu.set_cr3           = nested_svm_set_tdp_cr3;
	vcpu->arch.mmu.get_cr3           = nested_svm_get_tdp_cr3;
	vcpu->arch.mmu.get_pdptr         = nested_svm_get_tdp_pdptr;
	vcpu->arch.mmu.inject_page_fault = nested_svm_inject_npf_exit;
	vcpu->arch.mmu.shadow_root_level = get_npt_level();
	vcpu->arch.walk_mmu              = &vcpu->arch.nested_mmu;
}
bool kvm_hv_get_assist_page(struct kvm_vcpu *vcpu,
			    struct hv_vp_assist_page *assist_page)
{
	if (!kvm_hv_assist_page_enabled(vcpu))
		return false;
	return !kvm_read_guest_cached(vcpu->kvm, &vcpu->arch.pv_eoi.data,
				      assist_page, sizeof(*assist_page));
}
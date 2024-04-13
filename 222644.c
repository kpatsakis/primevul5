bool kvm_arch_can_inject_async_page_present(struct kvm_vcpu *vcpu)
{
	/*
	 * s390 will always inject the page directly,
	 * but we still want check_async_completion to cleanup
	 */
	return true;
}
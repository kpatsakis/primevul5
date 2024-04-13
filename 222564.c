static void kvm_gmap_notifier(struct gmap *gmap, unsigned long start,
			      unsigned long end)
{
	struct kvm *kvm = gmap->private;
	struct kvm_vcpu *vcpu;
	unsigned long prefix;
	int i;

	if (gmap_is_shadow(gmap))
		return;
	if (start >= 1UL << 31)
		/* We are only interested in prefix pages */
		return;
	kvm_for_each_vcpu(i, vcpu, kvm) {
		/* match against both prefix pages */
		prefix = kvm_s390_get_prefix(vcpu);
		if (prefix <= end && start <= prefix + 2*PAGE_SIZE - 1) {
			VCPU_EVENT(vcpu, 2, "gmap notifier for %lx-%lx",
				   start, end);
			kvm_s390_sync_request(KVM_REQ_MMU_RELOAD, vcpu);
		}
	}
}
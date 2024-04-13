static bool kvm_page_in_dirty_ring(struct kvm *kvm, unsigned long pgoff)
{
#if KVM_DIRTY_LOG_PAGE_OFFSET > 0
	return (pgoff >= KVM_DIRTY_LOG_PAGE_OFFSET) &&
	    (pgoff < KVM_DIRTY_LOG_PAGE_OFFSET +
	     kvm->dirty_ring_size / PAGE_SIZE);
#else
	return false;
#endif
}
static int avic_update_access_page(struct kvm *kvm, bool activate)
{
	int ret = 0;

	mutex_lock(&kvm->slots_lock);
	/*
	 * During kvm_destroy_vm(), kvm_pit_set_reinject() could trigger
	 * APICv mode change, which update APIC_ACCESS_PAGE_PRIVATE_MEMSLOT
	 * memory region. So, we need to ensure that kvm->mm == current->mm.
	 */
	if ((kvm->arch.apic_access_page_done == activate) ||
	    (kvm->mm != current->mm))
		goto out;

	ret = __x86_set_memory_region(kvm,
				      APIC_ACCESS_PAGE_PRIVATE_MEMSLOT,
				      APIC_DEFAULT_PHYS_BASE,
				      activate ? PAGE_SIZE : 0);
	if (ret)
		goto out;

	kvm->arch.apic_access_page_done = activate;
out:
	mutex_unlock(&kvm->slots_lock);
	return ret;
}
static int avic_vm_init(struct kvm *kvm)
{
	unsigned long flags;
	int err = -ENOMEM;
	struct kvm_svm *kvm_svm = to_kvm_svm(kvm);
	struct kvm_svm *k2;
	struct page *p_page;
	struct page *l_page;
	u32 vm_id;

	if (!avic)
		return 0;

	/* Allocating physical APIC ID table (4KB) */
	p_page = alloc_page(GFP_KERNEL_ACCOUNT);
	if (!p_page)
		goto free_avic;

	kvm_svm->avic_physical_id_table_page = p_page;
	clear_page(page_address(p_page));

	/* Allocating logical APIC ID table (4KB) */
	l_page = alloc_page(GFP_KERNEL_ACCOUNT);
	if (!l_page)
		goto free_avic;

	kvm_svm->avic_logical_id_table_page = l_page;
	clear_page(page_address(l_page));

	spin_lock_irqsave(&svm_vm_data_hash_lock, flags);
 again:
	vm_id = next_vm_id = (next_vm_id + 1) & AVIC_VM_ID_MASK;
	if (vm_id == 0) { /* id is 1-based, zero is not okay */
		next_vm_id_wrapped = 1;
		goto again;
	}
	/* Is it still in use? Only possible if wrapped at least once */
	if (next_vm_id_wrapped) {
		hash_for_each_possible(svm_vm_data_hash, k2, hnode, vm_id) {
			if (k2->avic_vm_id == vm_id)
				goto again;
		}
	}
	kvm_svm->avic_vm_id = vm_id;
	hash_add(svm_vm_data_hash, &kvm_svm->hnode, kvm_svm->avic_vm_id);
	spin_unlock_irqrestore(&svm_vm_data_hash_lock, flags);

	return 0;

free_avic:
	avic_vm_destroy(kvm);
	return err;
}
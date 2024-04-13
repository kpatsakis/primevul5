int kvm_arch_init_vm(struct kvm *kvm, unsigned long type)
{
	gfp_t alloc_flags = GFP_KERNEL;
	int i, rc;
	char debug_name[16];
	static unsigned long sca_offset;

	rc = -EINVAL;
#ifdef CONFIG_KVM_S390_UCONTROL
	if (type & ~KVM_VM_S390_UCONTROL)
		goto out_err;
	if ((type & KVM_VM_S390_UCONTROL) && (!capable(CAP_SYS_ADMIN)))
		goto out_err;
#else
	if (type)
		goto out_err;
#endif

	rc = s390_enable_sie();
	if (rc)
		goto out_err;

	rc = -ENOMEM;

	if (!sclp.has_64bscao)
		alloc_flags |= GFP_DMA;
	rwlock_init(&kvm->arch.sca_lock);
	/* start with basic SCA */
	kvm->arch.sca = (struct bsca_block *) get_zeroed_page(alloc_flags);
	if (!kvm->arch.sca)
		goto out_err;
	mutex_lock(&kvm_lock);
	sca_offset += 16;
	if (sca_offset + sizeof(struct bsca_block) > PAGE_SIZE)
		sca_offset = 0;
	kvm->arch.sca = (struct bsca_block *)
			((char *) kvm->arch.sca + sca_offset);
	mutex_unlock(&kvm_lock);

	sprintf(debug_name, "kvm-%u", current->pid);

	kvm->arch.dbf = debug_register(debug_name, 32, 1, 7 * sizeof(long));
	if (!kvm->arch.dbf)
		goto out_err;

	BUILD_BUG_ON(sizeof(struct sie_page2) != 4096);
	kvm->arch.sie_page2 =
	     (struct sie_page2 *) get_zeroed_page(GFP_KERNEL | GFP_DMA);
	if (!kvm->arch.sie_page2)
		goto out_err;

	kvm->arch.sie_page2->kvm = kvm;
	kvm->arch.model.fac_list = kvm->arch.sie_page2->fac_list;

	for (i = 0; i < kvm_s390_fac_size(); i++) {
		kvm->arch.model.fac_mask[i] = S390_lowcore.stfle_fac_list[i] &
					      (kvm_s390_fac_base[i] |
					       kvm_s390_fac_ext[i]);
		kvm->arch.model.fac_list[i] = S390_lowcore.stfle_fac_list[i] &
					      kvm_s390_fac_base[i];
	}
	kvm->arch.model.subfuncs = kvm_s390_available_subfunc;

	/* we are always in czam mode - even on pre z14 machines */
	set_kvm_facility(kvm->arch.model.fac_mask, 138);
	set_kvm_facility(kvm->arch.model.fac_list, 138);
	/* we emulate STHYI in kvm */
	set_kvm_facility(kvm->arch.model.fac_mask, 74);
	set_kvm_facility(kvm->arch.model.fac_list, 74);
	if (MACHINE_HAS_TLB_GUEST) {
		set_kvm_facility(kvm->arch.model.fac_mask, 147);
		set_kvm_facility(kvm->arch.model.fac_list, 147);
	}

	if (css_general_characteristics.aiv && test_facility(65))
		set_kvm_facility(kvm->arch.model.fac_mask, 65);

	kvm->arch.model.cpuid = kvm_s390_get_initial_cpuid();
	kvm->arch.model.ibc = sclp.ibc & 0x0fff;

	kvm_s390_crypto_init(kvm);

	mutex_init(&kvm->arch.float_int.ais_lock);
	spin_lock_init(&kvm->arch.float_int.lock);
	for (i = 0; i < FIRQ_LIST_COUNT; i++)
		INIT_LIST_HEAD(&kvm->arch.float_int.lists[i]);
	init_waitqueue_head(&kvm->arch.ipte_wq);
	mutex_init(&kvm->arch.ipte_mutex);

	debug_register_view(kvm->arch.dbf, &debug_sprintf_view);
	VM_EVENT(kvm, 3, "vm created with type %lu", type);

	if (type & KVM_VM_S390_UCONTROL) {
		kvm->arch.gmap = NULL;
		kvm->arch.mem_limit = KVM_S390_NO_MEM_LIMIT;
	} else {
		if (sclp.hamax == U64_MAX)
			kvm->arch.mem_limit = TASK_SIZE_MAX;
		else
			kvm->arch.mem_limit = min_t(unsigned long, TASK_SIZE_MAX,
						    sclp.hamax + 1);
		kvm->arch.gmap = gmap_create(current->mm, kvm->arch.mem_limit - 1);
		if (!kvm->arch.gmap)
			goto out_err;
		kvm->arch.gmap->private = kvm;
		kvm->arch.gmap->pfault_enabled = 0;
	}

	kvm->arch.use_pfmfi = sclp.has_pfmfi;
	kvm->arch.use_skf = sclp.has_skey;
	spin_lock_init(&kvm->arch.start_stop_lock);
	kvm_s390_vsie_init(kvm);
	if (use_gisa)
		kvm_s390_gisa_init(kvm);
	KVM_EVENT(3, "vm 0x%pK created by pid %u", kvm, current->pid);

	return 0;
out_err:
	free_page((unsigned long)kvm->arch.sie_page2);
	debug_unregister(kvm->arch.dbf);
	sca_dispose(kvm);
	KVM_EVENT(3, "creation of vm failed: %d", rc);
	return rc;
}
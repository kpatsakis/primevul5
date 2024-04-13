static int kvm_s390_get_machine(struct kvm *kvm, struct kvm_device_attr *attr)
{
	struct kvm_s390_vm_cpu_machine *mach;
	int ret = 0;

	mach = kzalloc(sizeof(*mach), GFP_KERNEL);
	if (!mach) {
		ret = -ENOMEM;
		goto out;
	}
	get_cpu_id((struct cpuid *) &mach->cpuid);
	mach->ibc = sclp.ibc;
	memcpy(&mach->fac_mask, kvm->arch.model.fac_mask,
	       S390_ARCH_FAC_LIST_SIZE_BYTE);
	memcpy((unsigned long *)&mach->fac_list, S390_lowcore.stfle_fac_list,
	       sizeof(S390_lowcore.stfle_fac_list));
	VM_EVENT(kvm, 3, "GET: host ibc:  0x%4.4x, host cpuid:  0x%16.16llx",
		 kvm->arch.model.ibc,
		 kvm->arch.model.cpuid);
	VM_EVENT(kvm, 3, "GET: host facmask:  0x%16.16llx.%16.16llx.%16.16llx",
		 mach->fac_mask[0],
		 mach->fac_mask[1],
		 mach->fac_mask[2]);
	VM_EVENT(kvm, 3, "GET: host faclist:  0x%16.16llx.%16.16llx.%16.16llx",
		 mach->fac_list[0],
		 mach->fac_list[1],
		 mach->fac_list[2]);
	if (copy_to_user((void __user *)attr->addr, mach, sizeof(*mach)))
		ret = -EFAULT;
	kfree(mach);
out:
	return ret;
}
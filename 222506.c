static int kvm_s390_get_processor(struct kvm *kvm, struct kvm_device_attr *attr)
{
	struct kvm_s390_vm_cpu_processor *proc;
	int ret = 0;

	proc = kzalloc(sizeof(*proc), GFP_KERNEL);
	if (!proc) {
		ret = -ENOMEM;
		goto out;
	}
	proc->cpuid = kvm->arch.model.cpuid;
	proc->ibc = kvm->arch.model.ibc;
	memcpy(&proc->fac_list, kvm->arch.model.fac_list,
	       S390_ARCH_FAC_LIST_SIZE_BYTE);
	VM_EVENT(kvm, 3, "GET: guest ibc: 0x%4.4x, guest cpuid: 0x%16.16llx",
		 kvm->arch.model.ibc,
		 kvm->arch.model.cpuid);
	VM_EVENT(kvm, 3, "GET: guest faclist: 0x%16.16llx.%16.16llx.%16.16llx",
		 kvm->arch.model.fac_list[0],
		 kvm->arch.model.fac_list[1],
		 kvm->arch.model.fac_list[2]);
	if (copy_to_user((void __user *)attr->addr, proc, sizeof(*proc)))
		ret = -EFAULT;
	kfree(proc);
out:
	return ret;
}
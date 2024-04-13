static int kvm_s390_set_processor(struct kvm *kvm, struct kvm_device_attr *attr)
{
	struct kvm_s390_vm_cpu_processor *proc;
	u16 lowest_ibc, unblocked_ibc;
	int ret = 0;

	mutex_lock(&kvm->lock);
	if (kvm->created_vcpus) {
		ret = -EBUSY;
		goto out;
	}
	proc = kzalloc(sizeof(*proc), GFP_KERNEL);
	if (!proc) {
		ret = -ENOMEM;
		goto out;
	}
	if (!copy_from_user(proc, (void __user *)attr->addr,
			    sizeof(*proc))) {
		kvm->arch.model.cpuid = proc->cpuid;
		lowest_ibc = sclp.ibc >> 16 & 0xfff;
		unblocked_ibc = sclp.ibc & 0xfff;
		if (lowest_ibc && proc->ibc) {
			if (proc->ibc > unblocked_ibc)
				kvm->arch.model.ibc = unblocked_ibc;
			else if (proc->ibc < lowest_ibc)
				kvm->arch.model.ibc = lowest_ibc;
			else
				kvm->arch.model.ibc = proc->ibc;
		}
		memcpy(kvm->arch.model.fac_list, proc->fac_list,
		       S390_ARCH_FAC_LIST_SIZE_BYTE);
		VM_EVENT(kvm, 3, "SET: guest ibc: 0x%4.4x, guest cpuid: 0x%16.16llx",
			 kvm->arch.model.ibc,
			 kvm->arch.model.cpuid);
		VM_EVENT(kvm, 3, "SET: guest faclist: 0x%16.16llx.%16.16llx.%16.16llx",
			 kvm->arch.model.fac_list[0],
			 kvm->arch.model.fac_list[1],
			 kvm->arch.model.fac_list[2]);
	} else
		ret = -EFAULT;
	kfree(proc);
out:
	mutex_unlock(&kvm->lock);
	return ret;
}
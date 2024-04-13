static int kvm_s390_set_processor_subfunc(struct kvm *kvm,
					  struct kvm_device_attr *attr)
{
	mutex_lock(&kvm->lock);
	if (kvm->created_vcpus) {
		mutex_unlock(&kvm->lock);
		return -EBUSY;
	}

	if (copy_from_user(&kvm->arch.model.subfuncs, (void __user *)attr->addr,
			   sizeof(struct kvm_s390_vm_cpu_subfunc))) {
		mutex_unlock(&kvm->lock);
		return -EFAULT;
	}
	mutex_unlock(&kvm->lock);

	VM_EVENT(kvm, 3, "SET: guest PLO    subfunc 0x%16.16lx.%16.16lx.%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.plo)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.plo)[1],
		 ((unsigned long *) &kvm->arch.model.subfuncs.plo)[2],
		 ((unsigned long *) &kvm->arch.model.subfuncs.plo)[3]);
	VM_EVENT(kvm, 3, "SET: guest PTFF   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.ptff)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.ptff)[1]);
	VM_EVENT(kvm, 3, "SET: guest KMAC   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmac)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmac)[1]);
	VM_EVENT(kvm, 3, "SET: guest KMC    subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmc)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmc)[1]);
	VM_EVENT(kvm, 3, "SET: guest KM     subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.km)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.km)[1]);
	VM_EVENT(kvm, 3, "SET: guest KIMD   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.kimd)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.kimd)[1]);
	VM_EVENT(kvm, 3, "SET: guest KLMD   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.klmd)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.klmd)[1]);
	VM_EVENT(kvm, 3, "SET: guest PCKMO  subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.pckmo)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.pckmo)[1]);
	VM_EVENT(kvm, 3, "SET: guest KMCTR  subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmctr)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmctr)[1]);
	VM_EVENT(kvm, 3, "SET: guest KMF    subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmf)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmf)[1]);
	VM_EVENT(kvm, 3, "SET: guest KMO    subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmo)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmo)[1]);
	VM_EVENT(kvm, 3, "SET: guest PCC    subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.pcc)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.pcc)[1]);
	VM_EVENT(kvm, 3, "SET: guest PPNO   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.ppno)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.ppno)[1]);
	VM_EVENT(kvm, 3, "SET: guest KMA    subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.kma)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.kma)[1]);
	VM_EVENT(kvm, 3, "SET: guest KDSA   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.kdsa)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.kdsa)[1]);
	VM_EVENT(kvm, 3, "SET: guest SORTL  subfunc 0x%16.16lx.%16.16lx.%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.sortl)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.sortl)[1],
		 ((unsigned long *) &kvm->arch.model.subfuncs.sortl)[2],
		 ((unsigned long *) &kvm->arch.model.subfuncs.sortl)[3]);
	VM_EVENT(kvm, 3, "SET: guest DFLTCC subfunc 0x%16.16lx.%16.16lx.%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.dfltcc)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.dfltcc)[1],
		 ((unsigned long *) &kvm->arch.model.subfuncs.dfltcc)[2],
		 ((unsigned long *) &kvm->arch.model.subfuncs.dfltcc)[3]);

	return 0;
}
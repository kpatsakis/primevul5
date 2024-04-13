static int kvm_s390_get_processor_subfunc(struct kvm *kvm,
					  struct kvm_device_attr *attr)
{
	if (copy_to_user((void __user *)attr->addr, &kvm->arch.model.subfuncs,
	    sizeof(struct kvm_s390_vm_cpu_subfunc)))
		return -EFAULT;

	VM_EVENT(kvm, 3, "GET: guest PLO    subfunc 0x%16.16lx.%16.16lx.%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.plo)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.plo)[1],
		 ((unsigned long *) &kvm->arch.model.subfuncs.plo)[2],
		 ((unsigned long *) &kvm->arch.model.subfuncs.plo)[3]);
	VM_EVENT(kvm, 3, "GET: guest PTFF   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.ptff)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.ptff)[1]);
	VM_EVENT(kvm, 3, "GET: guest KMAC   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmac)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmac)[1]);
	VM_EVENT(kvm, 3, "GET: guest KMC    subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmc)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmc)[1]);
	VM_EVENT(kvm, 3, "GET: guest KM     subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.km)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.km)[1]);
	VM_EVENT(kvm, 3, "GET: guest KIMD   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.kimd)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.kimd)[1]);
	VM_EVENT(kvm, 3, "GET: guest KLMD   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.klmd)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.klmd)[1]);
	VM_EVENT(kvm, 3, "GET: guest PCKMO  subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.pckmo)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.pckmo)[1]);
	VM_EVENT(kvm, 3, "GET: guest KMCTR  subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmctr)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmctr)[1]);
	VM_EVENT(kvm, 3, "GET: guest KMF    subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmf)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmf)[1]);
	VM_EVENT(kvm, 3, "GET: guest KMO    subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmo)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.kmo)[1]);
	VM_EVENT(kvm, 3, "GET: guest PCC    subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.pcc)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.pcc)[1]);
	VM_EVENT(kvm, 3, "GET: guest PPNO   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.ppno)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.ppno)[1]);
	VM_EVENT(kvm, 3, "GET: guest KMA    subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.kma)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.kma)[1]);
	VM_EVENT(kvm, 3, "GET: guest KDSA   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.kdsa)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.kdsa)[1]);
	VM_EVENT(kvm, 3, "GET: guest SORTL  subfunc 0x%16.16lx.%16.16lx.%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.sortl)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.sortl)[1],
		 ((unsigned long *) &kvm->arch.model.subfuncs.sortl)[2],
		 ((unsigned long *) &kvm->arch.model.subfuncs.sortl)[3]);
	VM_EVENT(kvm, 3, "GET: guest DFLTCC subfunc 0x%16.16lx.%16.16lx.%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm->arch.model.subfuncs.dfltcc)[0],
		 ((unsigned long *) &kvm->arch.model.subfuncs.dfltcc)[1],
		 ((unsigned long *) &kvm->arch.model.subfuncs.dfltcc)[2],
		 ((unsigned long *) &kvm->arch.model.subfuncs.dfltcc)[3]);

	return 0;
}
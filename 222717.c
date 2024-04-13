static int kvm_s390_get_machine_subfunc(struct kvm *kvm,
					struct kvm_device_attr *attr)
{
	if (copy_to_user((void __user *)attr->addr, &kvm_s390_available_subfunc,
	    sizeof(struct kvm_s390_vm_cpu_subfunc)))
		return -EFAULT;

	VM_EVENT(kvm, 3, "GET: host  PLO    subfunc 0x%16.16lx.%16.16lx.%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.plo)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.plo)[1],
		 ((unsigned long *) &kvm_s390_available_subfunc.plo)[2],
		 ((unsigned long *) &kvm_s390_available_subfunc.plo)[3]);
	VM_EVENT(kvm, 3, "GET: host  PTFF   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.ptff)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.ptff)[1]);
	VM_EVENT(kvm, 3, "GET: host  KMAC   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.kmac)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.kmac)[1]);
	VM_EVENT(kvm, 3, "GET: host  KMC    subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.kmc)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.kmc)[1]);
	VM_EVENT(kvm, 3, "GET: host  KM     subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.km)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.km)[1]);
	VM_EVENT(kvm, 3, "GET: host  KIMD   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.kimd)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.kimd)[1]);
	VM_EVENT(kvm, 3, "GET: host  KLMD   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.klmd)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.klmd)[1]);
	VM_EVENT(kvm, 3, "GET: host  PCKMO  subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.pckmo)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.pckmo)[1]);
	VM_EVENT(kvm, 3, "GET: host  KMCTR  subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.kmctr)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.kmctr)[1]);
	VM_EVENT(kvm, 3, "GET: host  KMF    subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.kmf)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.kmf)[1]);
	VM_EVENT(kvm, 3, "GET: host  KMO    subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.kmo)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.kmo)[1]);
	VM_EVENT(kvm, 3, "GET: host  PCC    subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.pcc)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.pcc)[1]);
	VM_EVENT(kvm, 3, "GET: host  PPNO   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.ppno)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.ppno)[1]);
	VM_EVENT(kvm, 3, "GET: host  KMA    subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.kma)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.kma)[1]);
	VM_EVENT(kvm, 3, "GET: host  KDSA   subfunc 0x%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.kdsa)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.kdsa)[1]);
	VM_EVENT(kvm, 3, "GET: host  SORTL  subfunc 0x%16.16lx.%16.16lx.%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.sortl)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.sortl)[1],
		 ((unsigned long *) &kvm_s390_available_subfunc.sortl)[2],
		 ((unsigned long *) &kvm_s390_available_subfunc.sortl)[3]);
	VM_EVENT(kvm, 3, "GET: host  DFLTCC subfunc 0x%16.16lx.%16.16lx.%16.16lx.%16.16lx",
		 ((unsigned long *) &kvm_s390_available_subfunc.dfltcc)[0],
		 ((unsigned long *) &kvm_s390_available_subfunc.dfltcc)[1],
		 ((unsigned long *) &kvm_s390_available_subfunc.dfltcc)[2],
		 ((unsigned long *) &kvm_s390_available_subfunc.dfltcc)[3]);

	return 0;
}
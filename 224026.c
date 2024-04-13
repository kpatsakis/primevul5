static __u64 power_pmu_bhrb_to(u64 addr)
{
	unsigned int instr;
	__u64 target;

	if (is_kernel_addr(addr)) {
		if (copy_from_kernel_nofault(&instr, (void *)addr,
				sizeof(instr)))
			return 0;

		return branch_target((struct ppc_inst *)&instr);
	}

	/* Userspace: need copy instruction here then translate it */
	if (copy_from_user_nofault(&instr, (unsigned int __user *)addr,
			sizeof(instr)))
		return 0;

	target = branch_target((struct ppc_inst *)&instr);
	if ((!target) || (instr & BRANCH_ABSOLUTE))
		return target;

	/* Translate relative branch target from kernel to user address */
	return target - (unsigned long)&instr + addr;
}
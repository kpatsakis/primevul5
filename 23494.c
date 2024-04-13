static void __init vdso_setup_syscall_map(void)
{
	unsigned int i;
	extern unsigned long *sys_call_table;
	extern unsigned long sys_ni_syscall;


	for (i = 0; i < __NR_syscalls; i++) {
#ifdef CONFIG_PPC64
		if (sys_call_table[i*2] != sys_ni_syscall)
			vdso_data->syscall_map_64[i >> 5] |=
				0x80000000UL >> (i & 0x1f);
		if (sys_call_table[i*2+1] != sys_ni_syscall)
			vdso_data->syscall_map_32[i >> 5] |=
				0x80000000UL >> (i & 0x1f);
#else /* CONFIG_PPC64 */
		if (sys_call_table[i] != sys_ni_syscall)
			vdso_data->syscall_map_32[i >> 5] |=
				0x80000000UL >> (i & 0x1f);
#endif /* CONFIG_PPC64 */
	}
}
asmlinkage long sys_getcpu(unsigned __user *cpup, unsigned __user *nodep,
	   		   struct getcpu_cache __user *cache)
{
	int err = 0;
	int cpu = raw_smp_processor_id();
	if (cpup)
		err |= put_user(cpu, cpup);
	if (nodep)
		err |= put_user(cpu_to_node(cpu), nodep);
	if (cache) {
		/*
		 * The cache is not needed for this implementation,
		 * but make sure user programs pass something
		 * valid. vsyscall implementations can instead make
		 * good use of the cache. Only use t0 and t1 because
		 * these are available in both 32bit and 64bit ABI (no
		 * need for a compat_getcpu). 32bit has enough
		 * padding
		 */
		unsigned long t0, t1;
		get_user(t0, &cache->blob[0]);
		get_user(t1, &cache->blob[1]);
		t0++;
		t1++;
		put_user(t0, &cache->blob[0]);
		put_user(t1, &cache->blob[1]);
	}
	return err ? -EFAULT : 0;
}
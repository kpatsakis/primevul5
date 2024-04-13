static void kvm_init_debug(void)
{
	struct kvm_stats_debugfs_item *p;

	kvm_debugfs_dir = debugfs_create_dir("kvm", NULL);

	kvm_debugfs_num_entries = 0;
	for (p = debugfs_entries; p->name; ++p, kvm_debugfs_num_entries++) {
		debugfs_create_file(p->name, KVM_DBGFS_GET_MODE(p),
				    kvm_debugfs_dir, (void *)(long)p->offset,
				    stat_fops[p->kind]);
	}
}
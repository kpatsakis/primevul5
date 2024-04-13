int register_reboot_notifier(struct notifier_block * nb)
{
	return blocking_notifier_chain_register(&reboot_notifier_list, nb);
}
int unregister_reboot_notifier(struct notifier_block * nb)
{
	return blocking_notifier_chain_unregister(&reboot_notifier_list, nb);
}
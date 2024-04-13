static void cil_reset_selinuxuser(struct cil_selinuxuser *selinuxuser)
{
	if (selinuxuser->range_str == NULL) {
		cil_reset_levelrange(selinuxuser->range);
	}
}
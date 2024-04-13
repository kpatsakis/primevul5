static void cil_reset_avrule(struct cil_avrule *rule)
{
	cil_reset_classperms_list(rule->perms.classperms);
}
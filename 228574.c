static void cil_reset_classmapping(struct cil_classmapping *cm)
{
	cil_reset_classperms_list(cm->classperms);
}
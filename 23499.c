const char *arch_vma_name(struct vm_area_struct *vma)
{
	if (vma->vm_mm && vma->vm_start == vma->vm_mm->context.vdso_base)
		return "[vdso]";
	return NULL;
}
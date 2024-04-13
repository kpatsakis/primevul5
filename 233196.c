static vm_fault_t vhost_vdpa_fault(struct vm_fault *vmf)
{
	struct vhost_vdpa *v = vmf->vma->vm_file->private_data;
	struct vdpa_device *vdpa = v->vdpa;
	const struct vdpa_config_ops *ops = vdpa->config;
	struct vdpa_notification_area notify;
	struct vm_area_struct *vma = vmf->vma;
	u16 index = vma->vm_pgoff;

	notify = ops->get_vq_notification(vdpa, index);

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	if (remap_pfn_range(vma, vmf->address & PAGE_MASK,
			    notify.addr >> PAGE_SHIFT, PAGE_SIZE,
			    vma->vm_page_prot))
		return VM_FAULT_SIGBUS;

	return VM_FAULT_NOPAGE;
}
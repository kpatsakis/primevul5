static int sclp_ctl_ioctl_sccb(void __user *user_area)
 {
 	struct sclp_ctl_sccb ctl_sccb;
 	struct sccb_header *sccb;
 	int rc;
 
 	if (copy_from_user(&ctl_sccb, user_area, sizeof(ctl_sccb)))
		return -EFAULT;
	if (!sclp_ctl_cmdw_supported(ctl_sccb.cmdw))
		return -EOPNOTSUPP;
 	sccb = (void *) get_zeroed_page(GFP_KERNEL | GFP_DMA);
 	if (!sccb)
 		return -ENOMEM;
	if (copy_from_user(sccb, u64_to_uptr(ctl_sccb.sccb), sizeof(*sccb))) {
 		rc = -EFAULT;
 		goto out_free;
 	}
	if (sccb->length > PAGE_SIZE || sccb->length < 8)
		return -EINVAL;
	if (copy_from_user(sccb, u64_to_uptr(ctl_sccb.sccb), sccb->length)) {
		rc = -EFAULT;
 		goto out_free;
 	}
 	rc = sclp_sync_request(ctl_sccb.cmdw, sccb);
	if (rc)
		goto out_free;
	if (copy_to_user(u64_to_uptr(ctl_sccb.sccb), sccb, sccb->length))
		rc = -EFAULT;
out_free:
	free_page((unsigned long) sccb);
	return rc;
}

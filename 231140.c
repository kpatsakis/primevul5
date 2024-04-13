bfad_im_issue_fc_host_lip(struct Scsi_Host *shost)
{
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_s *bfad = im_port->bfad;
	struct bfad_hal_comp fcomp;
	unsigned long flags;
	uint32_t status;

	init_completion(&fcomp.comp);
	spin_lock_irqsave(&bfad->bfad_lock, flags);
	status = bfa_port_disable(&bfad->bfa.modules.port,
					bfad_hcb_comp, &fcomp);
	spin_unlock_irqrestore(&bfad->bfad_lock, flags);

	if (status != BFA_STATUS_OK)
		return -EIO;

	wait_for_completion(&fcomp.comp);
	if (fcomp.status != BFA_STATUS_OK)
		return -EIO;

	spin_lock_irqsave(&bfad->bfad_lock, flags);
	status = bfa_port_enable(&bfad->bfa.modules.port,
					bfad_hcb_comp, &fcomp);
	spin_unlock_irqrestore(&bfad->bfad_lock, flags);
	if (status != BFA_STATUS_OK)
		return -EIO;

	wait_for_completion(&fcomp.comp);
	if (fcomp.status != BFA_STATUS_OK)
		return -EIO;

	return 0;
}
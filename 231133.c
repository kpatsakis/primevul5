bfad_im_reset_stats(struct Scsi_Host *shost)
{
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_s         *bfad = im_port->bfad;
	struct bfad_hal_comp fcomp;
	unsigned long   flags;
	bfa_status_t    rc;

	init_completion(&fcomp.comp);
	spin_lock_irqsave(&bfad->bfad_lock, flags);
	rc = bfa_port_clear_stats(BFA_FCPORT(&bfad->bfa), bfad_hcb_comp,
					&fcomp);
	spin_unlock_irqrestore(&bfad->bfad_lock, flags);

	if (rc != BFA_STATUS_OK)
		return;

	wait_for_completion(&fcomp.comp);

	return;
}
bfad_im_get_starget_port_name(struct scsi_target *starget)
{
	struct Scsi_Host *shost;
	struct bfad_im_port_s *im_port;
	struct bfad_s         *bfad;
	struct bfad_itnim_s   *itnim = NULL;
	u64             port_name = 0;
	unsigned long   flags;

	shost = dev_to_shost(starget->dev.parent);
	im_port = (struct bfad_im_port_s *) shost->hostdata[0];
	bfad = im_port->bfad;
	spin_lock_irqsave(&bfad->bfad_lock, flags);

	itnim = bfad_get_itnim(im_port, starget->id);
	if (itnim)
		port_name = bfa_fcs_itnim_get_pwwn(&itnim->fcs_itnim);

	fc_starget_port_name(starget) = cpu_to_be64(port_name);
	spin_unlock_irqrestore(&bfad->bfad_lock, flags);
}
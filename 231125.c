bfad_im_get_starget_port_id(struct scsi_target *starget)
{
	struct Scsi_Host *shost;
	struct bfad_im_port_s *im_port;
	struct bfad_s         *bfad;
	struct bfad_itnim_s   *itnim = NULL;
	u32        fc_id = -1;
	unsigned long   flags;

	shost = dev_to_shost(starget->dev.parent);
	im_port = (struct bfad_im_port_s *) shost->hostdata[0];
	bfad = im_port->bfad;
	spin_lock_irqsave(&bfad->bfad_lock, flags);

	itnim = bfad_get_itnim(im_port, starget->id);
	if (itnim)
		fc_id = bfa_fcs_itnim_get_fcid(&itnim->fcs_itnim);

	fc_starget_port_id(starget) = fc_id;
	spin_unlock_irqrestore(&bfad->bfad_lock, flags);
}
bfad_im_vport_disable(struct fc_vport *fc_vport, bool disable)
{
	struct bfad_vport_s *vport;
	struct bfad_s *bfad;
	struct bfa_fcs_vport_s *fcs_vport;
	struct Scsi_Host *vshost;
	wwn_t   pwwn;
	unsigned long flags;

	vport = (struct bfad_vport_s *)fc_vport->dd_data;
	bfad = vport->drv_port.bfad;
	vshost = vport->drv_port.im_port->shost;
	u64_to_wwn(fc_host_port_name(vshost), (u8 *)&pwwn);

	spin_lock_irqsave(&bfad->bfad_lock, flags);
	fcs_vport = bfa_fcs_vport_lookup(&bfad->bfa_fcs, 0, pwwn);
	spin_unlock_irqrestore(&bfad->bfad_lock, flags);

	if (fcs_vport == NULL)
		return VPCERR_BAD_WWN;

	if (disable) {
		bfa_fcs_vport_stop(fcs_vport);
		fc_vport_set_state(fc_vport, FC_VPORT_DISABLED);
	} else {
		bfa_fcs_vport_start(fcs_vport);
		fc_vport_set_state(fc_vport, FC_VPORT_ACTIVE);
	}

	return 0;
}
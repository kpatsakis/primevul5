bfad_im_vport_delete(struct fc_vport *fc_vport)
{
	struct bfad_vport_s *vport = (struct bfad_vport_s *)fc_vport->dd_data;
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) vport->drv_port.im_port;
	struct bfad_s *bfad = im_port->bfad;
	struct bfa_fcs_vport_s *fcs_vport;
	struct Scsi_Host *vshost;
	wwn_t   pwwn;
	int rc;
	unsigned long flags;
	struct completion fcomp;

	if (im_port->flags & BFAD_PORT_DELETE) {
		bfad_scsi_host_free(bfad, im_port);
		list_del(&vport->list_entry);
		kfree(vport);
		return 0;
	}

	vshost = vport->drv_port.im_port->shost;
	u64_to_wwn(fc_host_port_name(vshost), (u8 *)&pwwn);

	spin_lock_irqsave(&bfad->bfad_lock, flags);
	fcs_vport = bfa_fcs_vport_lookup(&bfad->bfa_fcs, 0, pwwn);
	spin_unlock_irqrestore(&bfad->bfad_lock, flags);

	if (fcs_vport == NULL)
		return VPCERR_BAD_WWN;

	vport->drv_port.flags |= BFAD_PORT_DELETE;

	vport->comp_del = &fcomp;
	init_completion(vport->comp_del);

	spin_lock_irqsave(&bfad->bfad_lock, flags);
	rc = bfa_fcs_vport_delete(&vport->fcs_vport);
	spin_unlock_irqrestore(&bfad->bfad_lock, flags);

	if (rc == BFA_STATUS_PBC) {
		vport->drv_port.flags &= ~BFAD_PORT_DELETE;
		vport->comp_del = NULL;
		return -1;
	}

	wait_for_completion(vport->comp_del);

	bfad_scsi_host_free(bfad, im_port);
	list_del(&vport->list_entry);
	kfree(vport);

	return 0;
}
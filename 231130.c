bfad_im_vport_create(struct fc_vport *fc_vport, bool disable)
{
	char *vname = fc_vport->symbolic_name;
	struct Scsi_Host *shost = fc_vport->shost;
	struct bfad_im_port_s *im_port =
		(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_s *bfad = im_port->bfad;
	struct bfa_lport_cfg_s port_cfg;
	struct bfad_vport_s *vp;
	int status = 0, rc;
	unsigned long flags;

	memset(&port_cfg, 0, sizeof(port_cfg));
	u64_to_wwn(fc_vport->node_name, (u8 *)&port_cfg.nwwn);
	u64_to_wwn(fc_vport->port_name, (u8 *)&port_cfg.pwwn);
	if (strlen(vname) > 0)
		strcpy((char *)&port_cfg.sym_name, vname);
	port_cfg.roles = BFA_LPORT_ROLE_FCP_IM;

	spin_lock_irqsave(&bfad->bfad_lock, flags);
	list_for_each_entry(vp, &bfad->pbc_vport_list, list_entry) {
		if (port_cfg.pwwn ==
				vp->fcs_vport.lport.port_cfg.pwwn) {
			port_cfg.preboot_vp =
				vp->fcs_vport.lport.port_cfg.preboot_vp;
			break;
		}
	}
	spin_unlock_irqrestore(&bfad->bfad_lock, flags);

	rc = bfad_vport_create(bfad, 0, &port_cfg, &fc_vport->dev);
	if (rc == BFA_STATUS_OK) {
		struct bfad_vport_s *vport;
		struct bfa_fcs_vport_s *fcs_vport;
		struct Scsi_Host *vshost;

		spin_lock_irqsave(&bfad->bfad_lock, flags);
		fcs_vport = bfa_fcs_vport_lookup(&bfad->bfa_fcs, 0,
					port_cfg.pwwn);
		spin_unlock_irqrestore(&bfad->bfad_lock, flags);
		if (fcs_vport == NULL)
			return VPCERR_BAD_WWN;

		fc_vport_set_state(fc_vport, FC_VPORT_ACTIVE);
		if (disable) {
			spin_lock_irqsave(&bfad->bfad_lock, flags);
			bfa_fcs_vport_stop(fcs_vport);
			spin_unlock_irqrestore(&bfad->bfad_lock, flags);
			fc_vport_set_state(fc_vport, FC_VPORT_DISABLED);
		}

		vport = fcs_vport->vport_drv;
		vshost = vport->drv_port.im_port->shost;
		fc_host_node_name(vshost) = wwn_to_u64((u8 *)&port_cfg.nwwn);
		fc_host_port_name(vshost) = wwn_to_u64((u8 *)&port_cfg.pwwn);
		fc_host_supported_classes(vshost) = FC_COS_CLASS3;

		memset(fc_host_supported_fc4s(vshost), 0,
			sizeof(fc_host_supported_fc4s(vshost)));

		/* For FCP type 0x08 */
		if (supported_fc4s & BFA_LPORT_ROLE_FCP_IM)
			fc_host_supported_fc4s(vshost)[2] = 1;

		/* For fibre channel services type 0x20 */
		fc_host_supported_fc4s(vshost)[7] = 1;

		fc_host_supported_speeds(vshost) =
				bfad_im_supported_speeds(&bfad->bfa);
		fc_host_maxframe_size(vshost) =
				bfa_fcport_get_maxfrsize(&bfad->bfa);

		fc_vport->dd_data = vport;
		vport->drv_port.im_port->fc_vport = fc_vport;
	} else if (rc == BFA_STATUS_INVALID_WWN)
		return VPCERR_BAD_WWN;
	else if (rc == BFA_STATUS_VPORT_EXISTS)
		return VPCERR_BAD_WWN;
	else if (rc == BFA_STATUS_VPORT_MAX)
		return VPCERR_NO_FABRIC_SUPP;
	else if (rc == BFA_STATUS_VPORT_WWN_BP)
		return VPCERR_BAD_WWN;
	else
		return FC_VPORT_FAILED;

	return status;
}
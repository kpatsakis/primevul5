bfad_im_get_host_port_type(struct Scsi_Host *shost)
{
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_s         *bfad = im_port->bfad;
	struct bfa_lport_attr_s port_attr;

	bfa_fcs_lport_get_attr(&bfad->bfa_fcs.fabric.bport, &port_attr);

	switch (port_attr.port_type) {
	case BFA_PORT_TYPE_NPORT:
		fc_host_port_type(shost) = FC_PORTTYPE_NPORT;
		break;
	case BFA_PORT_TYPE_NLPORT:
		fc_host_port_type(shost) = FC_PORTTYPE_NLPORT;
		break;
	case BFA_PORT_TYPE_P2P:
		fc_host_port_type(shost) = FC_PORTTYPE_PTP;
		break;
	case BFA_PORT_TYPE_LPORT:
		fc_host_port_type(shost) = FC_PORTTYPE_LPORT;
		break;
	default:
		fc_host_port_type(shost) = FC_PORTTYPE_UNKNOWN;
		break;
	}
}
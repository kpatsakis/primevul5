bfad_im_get_host_port_state(struct Scsi_Host *shost)
{
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_s         *bfad = im_port->bfad;
	struct bfa_port_attr_s attr;

	bfa_fcport_get_attr(&bfad->bfa, &attr);

	switch (attr.port_state) {
	case BFA_PORT_ST_LINKDOWN:
		fc_host_port_state(shost) = FC_PORTSTATE_LINKDOWN;
		break;
	case BFA_PORT_ST_LINKUP:
		fc_host_port_state(shost) = FC_PORTSTATE_ONLINE;
		break;
	case BFA_PORT_ST_DISABLED:
	case BFA_PORT_ST_STOPPED:
	case BFA_PORT_ST_IOCDOWN:
	case BFA_PORT_ST_IOCDIS:
		fc_host_port_state(shost) = FC_PORTSTATE_OFFLINE;
		break;
	case BFA_PORT_ST_UNINIT:
	case BFA_PORT_ST_ENABLING_QWAIT:
	case BFA_PORT_ST_ENABLING:
	case BFA_PORT_ST_DISABLING_QWAIT:
	case BFA_PORT_ST_DISABLING:
	default:
		fc_host_port_state(shost) = FC_PORTSTATE_UNKNOWN;
		break;
	}
}
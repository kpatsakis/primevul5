bfad_im_get_host_speed(struct Scsi_Host *shost)
{
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_s         *bfad = im_port->bfad;
	struct bfa_port_attr_s attr;

	bfa_fcport_get_attr(&bfad->bfa, &attr);
	switch (attr.speed) {
	case BFA_PORT_SPEED_10GBPS:
		fc_host_speed(shost) = FC_PORTSPEED_10GBIT;
		break;
	case BFA_PORT_SPEED_16GBPS:
		fc_host_speed(shost) = FC_PORTSPEED_16GBIT;
		break;
	case BFA_PORT_SPEED_8GBPS:
		fc_host_speed(shost) = FC_PORTSPEED_8GBIT;
		break;
	case BFA_PORT_SPEED_4GBPS:
		fc_host_speed(shost) = FC_PORTSPEED_4GBIT;
		break;
	case BFA_PORT_SPEED_2GBPS:
		fc_host_speed(shost) = FC_PORTSPEED_2GBIT;
		break;
	case BFA_PORT_SPEED_1GBPS:
		fc_host_speed(shost) = FC_PORTSPEED_1GBIT;
		break;
	default:
		fc_host_speed(shost) = FC_PORTSPEED_UNKNOWN;
		break;
	}
}
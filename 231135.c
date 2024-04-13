bfad_im_get_host_active_fc4s(struct Scsi_Host *shost)
{
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_port_s    *port = im_port->port;

	memset(fc_host_active_fc4s(shost), 0,
	       sizeof(fc_host_active_fc4s(shost)));

	if (port->supported_fc4s & BFA_LPORT_ROLE_FCP_IM)
		fc_host_active_fc4s(shost)[2] = 1;

	fc_host_active_fc4s(shost)[7] = 1;
}
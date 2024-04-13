bfad_im_get_host_fabric_name(struct Scsi_Host *shost)
{
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_port_s    *port = im_port->port;
	wwn_t           fabric_nwwn = 0;

	fabric_nwwn = bfa_fcs_lport_get_fabric_name(port->fcs_port);

	fc_host_fabric_name(shost) = cpu_to_be64(fabric_nwwn);

}
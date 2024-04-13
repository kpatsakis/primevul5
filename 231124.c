bfad_im_get_host_port_id(struct Scsi_Host *shost)
{
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_port_s    *port = im_port->port;

	fc_host_port_id(shost) =
			bfa_hton3b(bfa_fcs_lport_get_fcid(port->fcs_port));
}
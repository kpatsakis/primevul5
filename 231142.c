bfad_im_node_name_show(struct device *dev, struct device_attribute *attr,
				 char *buf)
{
	struct Scsi_Host *shost = class_to_shost(dev);
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_port_s    *port = im_port->port;
	u64        nwwn;

	nwwn = bfa_fcs_lport_get_nwwn(port->fcs_port);
	return snprintf(buf, PAGE_SIZE, "0x%llx\n", cpu_to_be64(nwwn));
}
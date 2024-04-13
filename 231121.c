bfad_im_symbolic_name_show(struct device *dev, struct device_attribute *attr,
				 char *buf)
{
	struct Scsi_Host *shost = class_to_shost(dev);
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_s *bfad = im_port->bfad;
	struct bfa_lport_attr_s port_attr;
	char symname[BFA_SYMNAME_MAXLEN];

	bfa_fcs_lport_get_attr(&bfad->bfa_fcs.fabric.bport, &port_attr);
	strlcpy(symname, port_attr.port_cfg.sym_name.symname,
			BFA_SYMNAME_MAXLEN);
	return snprintf(buf, PAGE_SIZE, "%s\n", symname);
}
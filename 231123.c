bfad_im_optionrom_version_show(struct device *dev,
			 struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(dev);
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_s *bfad = im_port->bfad;
	char optrom_ver[BFA_VERSION_LEN];

	bfa_get_adapter_optrom_ver(&bfad->bfa, optrom_ver);
	return snprintf(buf, PAGE_SIZE, "%s\n", optrom_ver);
}
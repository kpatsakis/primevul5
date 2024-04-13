bfad_im_fw_version_show(struct device *dev, struct device_attribute *attr,
				 char *buf)
{
	struct Scsi_Host *shost = class_to_shost(dev);
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_s *bfad = im_port->bfad;
	char fw_ver[BFA_VERSION_LEN];

	bfa_get_adapter_fw_ver(&bfad->bfa, fw_ver);
	return snprintf(buf, PAGE_SIZE, "%s\n", fw_ver);
}
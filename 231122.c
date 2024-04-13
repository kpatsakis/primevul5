bfad_im_hw_version_show(struct device *dev, struct device_attribute *attr,
				char *buf)
{
	struct Scsi_Host *shost = class_to_shost(dev);
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_s *bfad = im_port->bfad;
	char hw_ver[BFA_VERSION_LEN];

	bfa_get_pci_chip_rev(&bfad->bfa, hw_ver);
	return snprintf(buf, PAGE_SIZE, "%s\n", hw_ver);
}
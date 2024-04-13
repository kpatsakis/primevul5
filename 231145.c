bfad_im_num_of_ports_show(struct device *dev, struct device_attribute *attr,
				char *buf)
{
	struct Scsi_Host *shost = class_to_shost(dev);
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_s *bfad = im_port->bfad;

	return snprintf(buf, PAGE_SIZE, "%d\n",
			bfa_get_nports(&bfad->bfa));
}
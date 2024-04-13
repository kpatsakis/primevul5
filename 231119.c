bfad_im_serial_num_show(struct device *dev, struct device_attribute *attr,
			 char *buf)
{
	struct Scsi_Host *shost = class_to_shost(dev);
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_s *bfad = im_port->bfad;
	char serial_num[BFA_ADAPTER_SERIAL_NUM_LEN];

	bfa_get_adapter_serial_num(&bfad->bfa, serial_num);
	return snprintf(buf, PAGE_SIZE, "%s\n", serial_num);
}
bfad_im_num_of_discovered_ports_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct Scsi_Host *shost = class_to_shost(dev);
	struct bfad_im_port_s *im_port =
			(struct bfad_im_port_s *) shost->hostdata[0];
	struct bfad_port_s    *port = im_port->port;
	struct bfad_s         *bfad = im_port->bfad;
	int        nrports = 2048;
	struct bfa_rport_qualifier_s *rports = NULL;
	unsigned long   flags;

	rports = kcalloc(nrports, sizeof(struct bfa_rport_qualifier_s),
			 GFP_ATOMIC);
	if (rports == NULL)
		return snprintf(buf, PAGE_SIZE, "Failed\n");

	spin_lock_irqsave(&bfad->bfad_lock, flags);
	bfa_fcs_lport_get_rport_quals(port->fcs_port, rports, &nrports);
	spin_unlock_irqrestore(&bfad->bfad_lock, flags);
	kfree(rports);

	return snprintf(buf, PAGE_SIZE, "%d\n", nrports);
}
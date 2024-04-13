bfad_im_set_rport_loss_tmo(struct fc_rport *rport, u32 timeout)
{
	struct bfad_itnim_data_s *itnim_data = rport->dd_data;
	struct bfad_itnim_s   *itnim = itnim_data->itnim;
	struct bfad_s         *bfad = itnim->im->bfad;
	uint16_t path_tov = bfa_fcpim_path_tov_get(&bfad->bfa);

	rport->dev_loss_tmo = timeout;
	if (timeout < path_tov)
		rport->dev_loss_tmo = path_tov + 1;
}
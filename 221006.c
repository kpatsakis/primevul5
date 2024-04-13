dnp3_al_process_iin(tvbuff_t *tvb, packet_info *pinfo, int offset, proto_tree *al_tree)
{
  guint16     al_iin;
  proto_item *tiin;
  static int* const indications[] = {
      &hf_dnp3_al_iin_rst,
      &hf_dnp3_al_iin_dt,
      &hf_dnp3_al_iin_dol,
      &hf_dnp3_al_iin_tsr,
      &hf_dnp3_al_iin_cls3d,
      &hf_dnp3_al_iin_cls2d,
      &hf_dnp3_al_iin_cls1d,
      &hf_dnp3_al_iin_bmsg,
      &hf_dnp3_al_iin_cc,
      &hf_dnp3_al_iin_oae,
      &hf_dnp3_al_iin_ebo,
      &hf_dnp3_al_iin_pioor,
      &hf_dnp3_al_iin_obju,
      &hf_dnp3_al_iin_fcni,
      NULL
  };

  tiin = proto_tree_add_bitmask(al_tree, tvb, offset, hf_dnp3_al_iin, ett_dnp3_al_iin, indications, ENC_BIG_ENDIAN);
  al_iin = tvb_get_ntohs(tvb, offset);

  /* If IIN indicates an abnormal condition, add expert info */
  if ((al_iin & AL_IIN_DT) || (al_iin & AL_IIN_CC) || (al_iin & AL_IIN_OAE) || (al_iin & AL_IIN_EBO) ||
      (al_iin & AL_IIN_PIOOR) || (al_iin & AL_IIN_OBJU) || (al_iin & AL_IIN_FCNI)) {
      expert_add_info(pinfo, tiin, &ei_dnp_iin_abnormal);
  }
}
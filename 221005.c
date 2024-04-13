dnp3_al_obj_quality(tvbuff_t *tvb, int offset, guint8 al_ptflags, proto_tree *point_tree, proto_item *point_item, enum QUALITY_TYPE type)
{

  proto_tree *quality_tree;
  proto_item *quality_item;
  int         hf0 = 0, hf1 = 0, hf2 = 0, hf3 = 0, hf4 = 0, hf5 = 0, hf6 = 0, hf7 = 0;

  /* Common code */
  proto_item_append_text(point_item, " (Quality: ");
  quality_tree = proto_tree_add_subtree(point_tree, tvb, offset, 1, ett_dnp3_al_obj_quality, &quality_item, "Quality: ");

  if (al_ptflags & AL_OBJ_BI_FLAG0) {
    dnp3_append_2item_text(point_item, quality_item, "Online");
  }
  else {
    dnp3_append_2item_text(point_item, quality_item, "Offline");
  }
  if (al_ptflags & AL_OBJ_BI_FLAG1) dnp3_append_2item_text(point_item, quality_item, ", Restart");
  if (al_ptflags & AL_OBJ_BI_FLAG2) dnp3_append_2item_text(point_item, quality_item, ", Comm Fail");
  if (al_ptflags & AL_OBJ_BI_FLAG3) dnp3_append_2item_text(point_item, quality_item, ", Remote Force");
  if (al_ptflags & AL_OBJ_BI_FLAG4) dnp3_append_2item_text(point_item, quality_item, ", Local Force");

  switch (type) {
    case BIN_IN: /* Binary Input Quality flags */
      if (al_ptflags & AL_OBJ_BI_FLAG5) dnp3_append_2item_text(point_item, quality_item, ", Chatter Filter");

      hf0 = hf_dnp3_al_biq_b0;
      hf1 = hf_dnp3_al_biq_b1;
      hf2 = hf_dnp3_al_biq_b2;
      hf3 = hf_dnp3_al_biq_b3;
      hf4 = hf_dnp3_al_biq_b4;
      hf5 = hf_dnp3_al_biq_b5;
      hf6 = hf_dnp3_al_biq_b6;
      hf7 = hf_dnp3_al_biq_b7;
      break;

    case BIN_OUT: /* Binary Output Quality flags */
      hf0 = hf_dnp3_al_boq_b0;
      hf1 = hf_dnp3_al_boq_b1;
      hf2 = hf_dnp3_al_boq_b2;
      hf3 = hf_dnp3_al_boq_b3;
      hf4 = hf_dnp3_al_boq_b4;
      hf5 = hf_dnp3_al_boq_b5;
      hf6 = hf_dnp3_al_boq_b6;
      hf7 = hf_dnp3_al_boq_b7;
      break;

    case ANA_IN: /* Analog Input Quality flags */
      if (al_ptflags & AL_OBJ_AI_FLAG5) dnp3_append_2item_text(point_item, quality_item, ", Over-Range");
      if (al_ptflags & AL_OBJ_AI_FLAG6) dnp3_append_2item_text(point_item, quality_item, ", Reference Check");

      hf0 = hf_dnp3_al_aiq_b0;
      hf1 = hf_dnp3_al_aiq_b1;
      hf2 = hf_dnp3_al_aiq_b2;
      hf3 = hf_dnp3_al_aiq_b3;
      hf4 = hf_dnp3_al_aiq_b4;
      hf5 = hf_dnp3_al_aiq_b5;
      hf6 = hf_dnp3_al_aiq_b6;
      hf7 = hf_dnp3_al_aiq_b7;
      break;

    case ANA_OUT: /* Analog Output Quality flags */
      hf0 = hf_dnp3_al_aoq_b0;
      hf1 = hf_dnp3_al_aoq_b1;
      hf2 = hf_dnp3_al_aoq_b2;
      hf3 = hf_dnp3_al_aoq_b3;
      hf4 = hf_dnp3_al_aoq_b4;
      hf5 = hf_dnp3_al_aoq_b5;
      hf6 = hf_dnp3_al_aoq_b6;
      hf7 = hf_dnp3_al_aoq_b7;
      break;

    case COUNTER: /* Counter Quality flags */
      if (al_ptflags & AL_OBJ_CTR_FLAG5) dnp3_append_2item_text(point_item, quality_item, ", Roll-over");
      if (al_ptflags & AL_OBJ_CTR_FLAG6) dnp3_append_2item_text(point_item, quality_item, ", Discontinuity");

      hf0 = hf_dnp3_al_ctrq_b0;
      hf1 = hf_dnp3_al_ctrq_b1;
      hf2 = hf_dnp3_al_ctrq_b2;
      hf3 = hf_dnp3_al_ctrq_b3;
      hf4 = hf_dnp3_al_ctrq_b4;
      hf5 = hf_dnp3_al_ctrq_b5;
      hf6 = hf_dnp3_al_ctrq_b6;
      hf7 = hf_dnp3_al_ctrq_b7;
      break;
  }

  if (quality_tree != NULL) {
    proto_tree_add_item(quality_tree, hf7, tvb, offset, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(quality_tree, hf6, tvb, offset, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(quality_tree, hf5, tvb, offset, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(quality_tree, hf4, tvb, offset, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(quality_tree, hf3, tvb, offset, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(quality_tree, hf2, tvb, offset, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(quality_tree, hf1, tvb, offset, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item(quality_tree, hf0, tvb, offset, 1, ENC_LITTLE_ENDIAN);
  }
  proto_item_append_text(point_item, ")");
}
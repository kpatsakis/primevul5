dnp3_al_obj_procprefix(tvbuff_t *tvb, int offset, guint8 al_objq_prefix, guint32 *al_ptaddr, proto_tree *item_tree)
{
  int         prefixbytes = 0;
  proto_item *prefix_item;

  switch (al_objq_prefix)
  {
    case AL_OBJQL_PREFIX_NI:        /* No Prefix */
      prefixbytes = 0;
      prefix_item = proto_tree_add_uint(item_tree, hf_dnp3_al_point_index, tvb, offset, 0, *al_ptaddr);
      proto_item_set_generated(prefix_item);
      break;
    case AL_OBJQL_PREFIX_1O:
      *al_ptaddr = tvb_get_guint8(tvb, offset);
      proto_tree_add_item(item_tree, hf_dnp3_al_index8, tvb, offset, 1, ENC_LITTLE_ENDIAN);
      prefixbytes = 1;
      break;
    case AL_OBJQL_PREFIX_2O:
      *al_ptaddr = tvb_get_letohs(tvb, offset);
      proto_tree_add_item(item_tree, hf_dnp3_al_index16, tvb, offset, 2, ENC_LITTLE_ENDIAN);
      prefixbytes = 2;
      break;
    case AL_OBJQL_PREFIX_4O:
      *al_ptaddr = tvb_get_letohl(tvb, offset);
      proto_tree_add_item(item_tree, hf_dnp3_al_index32, tvb, offset, 4, ENC_LITTLE_ENDIAN);
      prefixbytes = 4;
      break;
    case AL_OBJQL_PREFIX_1OS:
      *al_ptaddr = tvb_get_guint8(tvb, offset);
      proto_tree_add_item(item_tree, hf_dnp3_al_size8, tvb, offset, 1, ENC_LITTLE_ENDIAN);
      prefixbytes = 1;
      break;
    case AL_OBJQL_PREFIX_2OS:
      *al_ptaddr = tvb_get_letohs(tvb, offset);
      proto_tree_add_item(item_tree, hf_dnp3_al_size16, tvb, offset, 2, ENC_LITTLE_ENDIAN);
      prefixbytes = 2;
      break;
    case AL_OBJQL_PREFIX_4OS:
      *al_ptaddr = tvb_get_letohl(tvb, offset);
      proto_tree_add_item(item_tree, hf_dnp3_al_size32, tvb, offset, 4, ENC_LITTLE_ENDIAN);
      prefixbytes = 4;
      break;
  }
  return prefixbytes;
}
dissect_dnp3_al(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
  guint8        al_ctl, al_seq, al_func, al_class = 0, i;
  guint16       bytes, obj_type = 0;
  guint         data_len = 0, offset = 0;
  proto_item   *ti, *tc;
  proto_tree   *al_tree, *robj_tree;
  const gchar  *func_code_str, *obj_type_str;
  nstime_t      al_cto;
  static int * const control_flags[] = {
    &hf_dnp3_al_fir,
    &hf_dnp3_al_fin,
    &hf_dnp3_al_con,
    &hf_dnp3_al_uns,
    &hf_dnp3_al_seq,
    NULL
  };

  nstime_set_zero (&al_cto);

  data_len = tvb_captured_length(tvb);

  /* Handle the control byte and function code */
  al_ctl = tvb_get_guint8(tvb, offset);
  al_seq = al_ctl & DNP3_AL_SEQ;
  al_func = tvb_get_guint8(tvb, (offset+1));
  func_code_str = val_to_str_ext(al_func, &dnp3_al_func_vals_ext, "Unknown function (0x%02x)");

  /* Clear out lower layer info */
  col_clear(pinfo->cinfo, COL_INFO);
  col_append_sep_str(pinfo->cinfo, COL_INFO, NULL, func_code_str);
  col_set_fence(pinfo->cinfo, COL_INFO);

  /* format up the text representation */
  al_tree = proto_tree_add_subtree(tree, tvb, offset, data_len, ett_dnp3_al, &ti, "Application Layer: (");
  if (al_ctl & DNP3_AL_FIR)  proto_item_append_text(ti, "FIR, ");
  if (al_ctl & DNP3_AL_FIN)  proto_item_append_text(ti, "FIN, ");
  if (al_ctl & DNP3_AL_CON)  proto_item_append_text(ti, "CON, ");
  if (al_ctl & DNP3_AL_UNS)  proto_item_append_text(ti, "UNS, ");
  proto_item_append_text(ti, "Sequence %u, %s)", al_seq, func_code_str);

  /* Application Layer control byte subtree */
  tc = proto_tree_add_bitmask(al_tree, tvb, offset, hf_dnp3_al_ctl, ett_dnp3_al_ctl, control_flags, ENC_BIG_ENDIAN);
  proto_item_append_text(tc, "(");
  if (al_ctl & DNP3_AL_FIR)  proto_item_append_text(tc, "FIR, ");
  if (al_ctl & DNP3_AL_FIN)  proto_item_append_text(tc, "FIN, ");
  if (al_ctl & DNP3_AL_CON)  proto_item_append_text(tc, "CON, ");
  if (al_ctl & DNP3_AL_UNS)  proto_item_append_text(tc, "UNS, ");
  proto_item_append_text(tc, "Sequence %u)", al_seq);
  offset += 1;

#if 0
  /* If this packet is NOT the final Application Layer Message, exit and continue
     processing the remaining data in the fragment. */
  if (!(al_ctl & DNP3_AL_FIN)) {
    t_robj = proto_tree_add_expert(al_tree, pinfo, &ei_dnp3_buffering_user_data_until_final_frame_is_received, tvb, offset, -1);
    return 1;
  }
#endif

  /* Application Layer Function Code Byte  */
  proto_tree_add_uint_format(al_tree, hf_dnp3_al_func, tvb, offset, 1, al_func,
    "Function Code: %s (0x%02x)", func_code_str, al_func);
  offset += 1;

  switch (al_func)
  {
    case AL_FUNC_READ:     /* Read Function Code 0x01 */

      /* Create Read Request Data Objects Tree */
      robj_tree = proto_tree_add_subtree(al_tree, tvb, offset, -1, ett_dnp3_al_objdet, NULL, "READ Request Data Objects");

      /* Process Data Object Details */
      while (offset <= (data_len-2))  {  /* 2 octet object code + CRC32 */
        offset = dnp3_al_process_object(tvb, pinfo, offset, robj_tree, TRUE, &obj_type, &al_cto);

        /* Update class type for each object that was a class read */
        switch(obj_type) {
          case AL_OBJ_CLASS0:
          case AL_OBJ_CLASS1:
          case AL_OBJ_CLASS2:
          case AL_OBJ_CLASS3:
            al_class |= (1 << ((obj_type & 0x0f) - 1));
            break;
          default:
            /* For reads for specific object types, bit-mask out the first byte and add the generic obj description to the column info */
            obj_type_str = val_to_str_ext((obj_type & 0xFF00), &dnp3_al_read_obj_vals_ext, "Unknown Object Type");
            col_append_sep_str(pinfo->cinfo, COL_INFO, NULL, obj_type_str);
            break;
        }

      }

      /* Update the col info if there were class reads */
      if (al_class != 0) {
        col_append_sep_str(pinfo->cinfo, COL_INFO, NULL, "Class ");
        for (i = 0; i < 4; i++) {
          if (al_class & (1 << i)) {
            col_append_fstr(pinfo->cinfo, COL_INFO, "%u", i);
          }
        }
      }

      break;

    case AL_FUNC_WRITE:     /* Write Function Code 0x02 */

      /* Create Write Request Data Objects Tree */
      robj_tree = proto_tree_add_subtree(al_tree, tvb, offset, -1, ett_dnp3_al_objdet, NULL, "WRITE Request Data Objects");

      /* Process Data Object Details */
      while (offset <= (data_len-2))  {  /* 2 octet object code + CRC32 */
        offset = dnp3_al_process_object(tvb, pinfo, offset, robj_tree, FALSE, &obj_type, &al_cto);

        /* For writes for specific object types, bit-mask out the first byte and add the generic obj description to the column info */
        obj_type_str = val_to_str_ext((obj_type & 0xFF00), &dnp3_al_write_obj_vals_ext, "Unknown Object Type");
        col_append_sep_str(pinfo->cinfo, COL_INFO, NULL, obj_type_str);

      }

      break;

    case AL_FUNC_SELECT:     /* Select Function Code 0x03 */

      /* Create Select Request Data Objects Tree */
      robj_tree = proto_tree_add_subtree(al_tree, tvb, offset, -1, ett_dnp3_al_objdet, NULL, "SELECT Request Data Objects");

      /* Process Data Object Details */
      while (offset <= (data_len-2))  {  /* 2 octet object code + CRC32 */
        offset = dnp3_al_process_object(tvb, pinfo, offset, robj_tree, FALSE, &obj_type, &al_cto);
      }

      break;

    case AL_FUNC_OPERATE:    /* Operate Function Code 0x04 */
      /* Functionally identical to 'SELECT' Function Code */

      /* Create Operate Request Data Objects Tree */
      robj_tree = proto_tree_add_subtree(al_tree, tvb, offset, -1, ett_dnp3_al_objdet, NULL, "OPERATE Request Data Objects");

      /* Process Data Object Details */
      while (offset <= (data_len-2))  {  /* 2 octet object code + CRC32 */
        offset = dnp3_al_process_object(tvb, pinfo, offset, robj_tree, FALSE, &obj_type, &al_cto);
      }

      break;

    case AL_FUNC_DIROP:        /* Direct Operate Function Code 0x05 */
    case AL_FUNC_DIROPNACK:    /* Direct Operate No ACK Function Code 0x06 */
      /* Functionally identical to 'SELECT' Function Code */

      /* Create Direct Operate Request Data Objects Tree */
      robj_tree = proto_tree_add_subtree(al_tree, tvb, offset, -1, ett_dnp3_al_objdet, NULL, "DIRECT OPERATE Request Data Objects");

      /* Process Data Object Details */
      while (offset <= (data_len-2))  {  /* 2 octet object code + CRC32 */
        offset = dnp3_al_process_object(tvb, pinfo, offset, robj_tree, FALSE, &obj_type, &al_cto);
      }

      break;

    case AL_FUNC_FRZ:        /* Immediate Freeze Function Code 0x07 */
    case AL_FUNC_FRZNACK:    /* Immediate Freeze No ACK Function Code 0x08 */
    case AL_FUNC_FRZCLR:     /* Freeze and Clear Function Code 0x09 */
    case AL_FUNC_FRZCLRNACK: /* Freeze and Clear No ACK Function Code 0x0A */

      /* Create Freeze Request Data Objects Tree */
      robj_tree = proto_tree_add_subtree(al_tree, tvb, offset, -1, ett_dnp3_al_objdet, NULL, "Freeze Request Data Objects");

      /* Process Data Object Details */
      while (offset <= (data_len-2))  {  /* 2 octet object code + CRC32 */
        offset = dnp3_al_process_object(tvb, pinfo, offset, robj_tree, TRUE, &obj_type, &al_cto);
      }

      break;

    case AL_FUNC_ENSPMSG:   /* Enable Spontaneous Messages Function Code 0x14 */

      /* Create Enable Spontaneous Messages Data Objects Tree */
      robj_tree = proto_tree_add_subtree(al_tree, tvb, offset, -1, ett_dnp3_al_objdet, NULL, "Enable Spontaneous Msg's Data Objects");

      /* Process Data Object Details */
      while (offset <= (data_len-2))  {  /* 2 octet object code + CRC32 */
        offset = dnp3_al_process_object(tvb, pinfo, offset, robj_tree, FALSE, &obj_type, &al_cto);
      }

      break;

    case AL_FUNC_DISSPMSG:   /* Disable Spontaneous Messages Function Code 0x15 */

      /* Create Disable Spontaneous Messages Data Objects Tree */
      robj_tree = proto_tree_add_subtree(al_tree, tvb, offset, -1, ett_dnp3_al_objdet, NULL, "Disable Spontaneous Msg's Data Objects");

      /* Process Data Object Details */
      while (offset <= (data_len-2))  {  /* 2 octet object code + CRC32 */
        offset = dnp3_al_process_object(tvb, pinfo, offset, robj_tree, FALSE, &obj_type, &al_cto);
      }

      break;

    case AL_FUNC_DELAYMST:  /* Delay Measurement Function Code 0x17 */

      break;

    case AL_FUNC_OPENFILE:        /* Open File Function Code 0x19 */
    case AL_FUNC_CLOSEFILE:       /* Close File Function Code 0x1A */
    case AL_FUNC_DELETEFILE:      /* Delete File Function Code 0x1B */

      /* Create File Data Objects Tree */
      robj_tree = proto_tree_add_subtree(al_tree, tvb, offset, -1, ett_dnp3_al_objdet, NULL, "File Data Objects");

      /* Process Data Object Details */
      while (offset <= (data_len-2))  {  /* 2 octet object code + CRC32 */
        offset = dnp3_al_process_object(tvb, pinfo, offset, robj_tree, FALSE, &obj_type, &al_cto);
      }

      break;

    case AL_FUNC_RESPON:   /* Response Function Code 0x81 */
    case AL_FUNC_UNSOLI:   /* Unsolicited Response Function Code 0x82 */

      /* Application Layer IIN bits req'd if message is a response */
      dnp3_al_process_iin(tvb, pinfo, offset, al_tree);
      offset += 2;

      /* Ensure there is actual data remaining in the message.
         A response will not contain data following the IIN bits,
         if there is none available */
      bytes = tvb_reported_length_remaining(tvb, offset);
      if (bytes > 0)
      {
        /* Create Response Data Objects Tree */
        robj_tree = proto_tree_add_subtree(al_tree, tvb, offset, -1, ett_dnp3_al_objdet, NULL, "RESPONSE Data Objects");

        /* Process Data Object Details */
        while (offset <= (data_len-2)) {  /* 2 octet object code + CRC32 */
          offset = dnp3_al_process_object(tvb, pinfo, offset, robj_tree, FALSE, &obj_type, &al_cto);
        }

        break;
      }

    default:    /* Unknown Function */

      break;
  }

  return 0;
}
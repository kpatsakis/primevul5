dnp3_al_process_object(tvbuff_t *tvb, packet_info *pinfo, int offset,
                       proto_tree *robj_tree, gboolean header_only,
                       guint16 *al_objtype, nstime_t *al_cto)
{

  guint8      al_objq, al_objq_prefix, al_objq_range, al_oct_len = 0, bitindex;
  guint16     al_obj, temp;
  guint32     al_ptaddr = 0;
  int         num_items = 0;
  int         orig_offset, rangebytes = 0;
  proto_item *object_item, *range_item;
  proto_tree *object_tree, *qualifier_tree, *range_tree;

  orig_offset = offset;

  /* Application Layer Objects in this Message */
  *al_objtype =
  al_obj = tvb_get_ntohs(tvb, offset);

  /* Special handling for Octet string objects as the variation is the length of the string */
  temp = al_obj & 0xFF00;
  if ((temp == AL_OBJ_OCT) || (temp == AL_OBJ_OCT_EVT )) {
    al_oct_len = al_obj & 0xFF;
    al_obj = temp;
  }

  /* Create Data Objects Detail Tree */
  if (AL_OBJ_GROUP(al_obj) == 0x0) {
    object_item = proto_tree_add_uint_format(robj_tree, hf_dnp3_al_obj, tvb, offset, 2, al_obj,
                                             "Object(s): %s (0x%04x)",
                                             val_to_str_ext_const(al_obj, &dnp3_al_obj_vals_ext, "Unknown group 0 Variation"),
                                             al_obj);
    if (try_val_to_str_ext(al_obj, &dnp3_al_obj_vals_ext) == NULL) {
      expert_add_info(pinfo, object_item, &ei_dnp3_unknown_group0_variation);
    }
  }
  else {
    object_item = proto_tree_add_uint_format(robj_tree, hf_dnp3_al_obj, tvb, offset, 2, al_obj,
                                             "Object(s): %s (0x%04x)",
                                             val_to_str_ext_const(al_obj, &dnp3_al_obj_vals_ext, "Unknown Object\\Variation"),
                                             al_obj);
    if (try_val_to_str_ext(al_obj, &dnp3_al_obj_vals_ext) == NULL) {
      expert_add_info(pinfo, object_item, &ei_dnp3_unknown_object);
    }
  }
  object_tree = proto_item_add_subtree(object_item, ett_dnp3_al_obj);

  offset += 2;

  /* Object Qualifier */
  al_objq = tvb_get_guint8(tvb, offset);
  al_objq_prefix = al_objq & AL_OBJQ_PREFIX;
  al_objq_prefix = al_objq_prefix >> 4;
  al_objq_range = al_objq & AL_OBJQ_RANGE;

  qualifier_tree = proto_tree_add_subtree_format(object_tree, tvb, offset, 1, ett_dnp3_al_obj_qualifier, NULL,
    "Qualifier Field, Prefix: %s, Range: %s",
    val_to_str_ext_const(al_objq_prefix, &dnp3_al_objq_prefix_vals_ext, "Unknown Prefix Type"),
    val_to_str_ext_const(al_objq_range, &dnp3_al_objq_range_vals_ext, "Unknown Range Type"));
  proto_tree_add_item(qualifier_tree, hf_dnp3_al_objq_prefix, tvb, offset, 1, ENC_BIG_ENDIAN);
  proto_tree_add_item(qualifier_tree, hf_dnp3_al_objq_range, tvb, offset, 1, ENC_BIG_ENDIAN);

  offset += 1;

  /* Create (possibly synthesized) number of items and range field tree */
  range_tree = proto_tree_add_subtree(object_tree, tvb, offset, 0, ett_dnp3_al_obj_range, &range_item, "Number of Items: ");

  switch (al_objq_range)
  {
    case AL_OBJQL_RANGE_SSI8:           /* 8-bit Start and Stop Indices in Range Field */
      num_items = ( tvb_get_guint8(tvb, offset+1) - tvb_get_guint8(tvb, offset) + 1);
      proto_item_set_generated(range_item);
      al_ptaddr = tvb_get_guint8(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_start8, tvb, offset, 1, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_stop8, tvb, offset + 1, 1, ENC_LITTLE_ENDIAN);
      rangebytes = 2;
      break;
    case AL_OBJQL_RANGE_SSI16:          /* 16-bit Start and Stop Indices in Range Field */
      num_items = ( tvb_get_letohs(tvb, offset+2) - tvb_get_letohs(tvb, (offset)) + 1);
      proto_item_set_generated(range_item);
      al_ptaddr = tvb_get_letohs(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_start16, tvb, offset, 2, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_stop16, tvb, offset + 2, 2, ENC_LITTLE_ENDIAN);
      rangebytes = 4;
      break;
    case AL_OBJQL_RANGE_SSI32:          /* 32-bit Start and Stop Indices in Range Field */
      num_items = ( tvb_get_letohl(tvb, offset+4) - tvb_get_letohl(tvb, offset) + 1);
      proto_item_set_generated(range_item);
      al_ptaddr = tvb_get_letohl(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_start32, tvb, offset, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_stop32, tvb, offset + 4, 4, ENC_LITTLE_ENDIAN);
      rangebytes = 8;
      break;
    case AL_OBJQL_RANGE_AA8:            /* 8-bit Absolute Address in Range Field */
      num_items = 1;
      proto_item_set_generated(range_item);
      al_ptaddr = tvb_get_guint8(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_abs8, tvb, offset, 1, ENC_LITTLE_ENDIAN);
      rangebytes = 1;
      break;
    case AL_OBJQL_RANGE_AA16:           /* 16-bit Absolute Address in Range Field */
      num_items = 1;
      proto_item_set_generated(range_item);
      al_ptaddr = tvb_get_letohs(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_abs16, tvb, offset, 2, ENC_LITTLE_ENDIAN);
      rangebytes = 2;
      break;
    case AL_OBJQL_RANGE_AA32:           /* 32-bit Absolute Address in Range Field */
      num_items = 1;
      proto_item_set_generated(range_item);
      al_ptaddr = tvb_get_letohl(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_abs32, tvb, offset, 4, ENC_LITTLE_ENDIAN);
      rangebytes = 4;
      break;
    case AL_OBJQL_RANGE_SF8:            /* 8-bit Single Field Quantity in Range Field */
      num_items = tvb_get_guint8(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_quant8, tvb, offset, 1, ENC_LITTLE_ENDIAN);
      rangebytes = 1;
      proto_item_set_len(range_item, rangebytes);
      break;
    case AL_OBJQL_RANGE_SF16:           /* 16-bit Single Field Quantity in Range Field */
      num_items = tvb_get_letohs(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_quant16, tvb, offset, 2, ENC_LITTLE_ENDIAN);
      rangebytes = 2;
      proto_item_set_len(range_item, rangebytes);
      break;
    case AL_OBJQL_RANGE_SF32:           /* 32-bit Single Field Quantity in Range Field */
      num_items = tvb_get_letohl(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_quant32, tvb, offset, 4, ENC_LITTLE_ENDIAN);
      rangebytes = 4;
      proto_item_set_len(range_item, rangebytes);
      break;
    case AL_OBJQL_RANGE_FF:            /* 8 bit object count in Range Field */
      num_items = tvb_get_guint8(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_quant8, tvb, offset, 1, ENC_LITTLE_ENDIAN);
      rangebytes = 1;
      proto_item_set_len(range_item, rangebytes);
  }
  if (num_items > 0) {
    proto_item_append_text(object_item, ", %d point%s", num_items, plurality(num_items, "", "s"));
  }
  proto_item_append_text(range_item, "%d", num_items);

  /* A negative number of items is an error */
  if (num_items < 0) {
    proto_item_append_text(range_item, " (bogus)");
    expert_add_info(pinfo, range_item, &ei_dnp_num_items_neg);
    return tvb_captured_length(tvb);
  }

  /* Move offset past any range field */
  offset += rangebytes;

  bitindex = 0; /* Temp variable for cycling through points when object values are encoded into
                   bits; primarily objects 0x0101, 0x0301 & 0x1001 */

  /* Only process the point information for replies or items with point index lists */
  if (!header_only || al_objq_prefix > 0) {
    int item_num;
    int start_offset;

    start_offset = offset;
    for (item_num = 0; item_num < num_items; item_num++)
    {
      proto_item *point_item;
      proto_tree *point_tree;
      guint       data_pos;
      int         prefixbytes;

      /* Create Point item and process prefix */
      if (al_objq_prefix <= AL_OBJQL_PREFIX_4O) {
        point_tree = proto_tree_add_subtree(object_tree, tvb, offset, -1, ett_dnp3_al_obj_point, &point_item, "Point Number");
      }
      else {
        point_tree = proto_tree_add_subtree(object_tree, tvb, offset, -1, ett_dnp3_al_obj_point, &point_item, "Object: Size");
      }

      data_pos   = offset;
      prefixbytes = dnp3_al_obj_procprefix(tvb, offset, al_objq_prefix, &al_ptaddr, point_tree);

      /* If this is an 'empty' object type and the num_items field is not equal to zero,
         then the packet is potentially malicious */
      if ((num_items != 0) && (dnp3_al_empty_obj(al_obj))) {
        proto_item_append_text(range_item, " (bogus)");
        expert_add_info(pinfo, range_item, &ei_dnp3_num_items_invalid);
        num_items = 0;
      }

      proto_item_append_text(point_item, " %u", al_ptaddr);
      proto_item_set_len(point_item, prefixbytes);
      data_pos += prefixbytes;

      if (!header_only || (AL_OBJQL_PREFIX_1OS <= al_objq_prefix && al_objq_prefix <= AL_OBJQL_PREFIX_4OS)) {
        /* Process the object values */
        guint8       al_2bit, al_ptflags, al_bi_val, al_tcc_code;
        gint16       al_val_int16;
        guint16      al_val_uint16, al_ctlobj_stat;
        guint16      al_relms, al_filename_len, al_file_ctrl_mode;
        gint32       al_val_int32;
        guint32      al_val_uint32, file_data_size;
        nstime_t     al_reltime, al_abstime;
        gboolean     al_bit;
        gfloat       al_valflt;
        gdouble      al_valdbl;
        const gchar *ctl_status_str;

        /* Device Attributes (g0) all have a type code, use that rather than the individual variation */
        if (AL_OBJ_GROUP(al_obj) == 0x0) {
          guint32 data_type;
          guint8 da_len;

          /* Add and retrieve the data type */
          proto_tree_add_item_ret_uint(point_tree, hf_dnp3_al_datatype, tvb, data_pos, 1, ENC_LITTLE_ENDIAN, &data_type);
          data_pos++;

          /* If a valid data type process it */
          if (try_val_to_str(data_type, dnp3_al_data_type_vals) != NULL) {
            switch(data_type) {
              case AL_DATA_TYPE_NONE:
                break;
              case AL_DATA_TYPE_VSTR:
                da_len = tvb_get_guint8(tvb, data_pos);
                proto_tree_add_item(point_tree, hf_dnp3_al_da_length, tvb, data_pos, 1, ENC_LITTLE_ENDIAN);
                data_pos++;
                const guint8* da_value;
                proto_tree_add_item_ret_string(point_tree, hf_dnp3_al_da_value, tvb, data_pos, da_len, ENC_ASCII|ENC_NA, wmem_packet_scope(), &da_value);
                proto_item_append_text(object_item, ", Value: %s", da_value);
                data_pos += da_len;
                break;
              case AL_DATA_TYPE_UINT:
                da_len = tvb_get_guint8(tvb, data_pos);
                proto_tree_add_item(point_tree, hf_dnp3_al_da_length, tvb, data_pos, 1, ENC_LITTLE_ENDIAN);
                data_pos++;
                if (da_len == 1) {
                  proto_tree_add_item(point_tree, hf_dnp3_al_da_uint8, tvb, data_pos, 1, ENC_LITTLE_ENDIAN);
                  proto_item_append_text(object_item, ", Value: %u", tvb_get_guint8(tvb, data_pos));
                  data_pos++;
                }
                else if (da_len == 2) {
                  proto_tree_add_item(point_tree, hf_dnp3_al_da_uint16, tvb, data_pos, 2, ENC_LITTLE_ENDIAN);
                  proto_item_append_text(object_item, ", Value: %u", tvb_get_letohs(tvb, data_pos));
                  data_pos += 2;
                }
                else if (da_len == 4) {
                  proto_tree_add_item(point_tree, hf_dnp3_al_da_uint32, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
                  proto_item_append_text(object_item, ", Value: %u", tvb_get_letohl(tvb, data_pos));
                  data_pos += 4;
                }
                break;
              case AL_DATA_TYPE_INT:
                da_len = tvb_get_guint8(tvb, data_pos);
                proto_tree_add_item(point_tree, hf_dnp3_al_da_length, tvb, data_pos, 1, ENC_LITTLE_ENDIAN);
                data_pos++;
                if (da_len == 1) {
                  proto_tree_add_item(point_tree, hf_dnp3_al_da_int8, tvb, data_pos, 1, ENC_LITTLE_ENDIAN);
                  proto_item_append_text(object_item, ", Value: %d", tvb_get_guint8(tvb, data_pos));
                  data_pos++;
                }
                else if (da_len == 2) {
                  proto_tree_add_item(point_tree, hf_dnp3_al_da_int16, tvb, data_pos, 2, ENC_LITTLE_ENDIAN);
                  proto_item_append_text(object_item, ", Value: %d", tvb_get_letohs(tvb, data_pos));
                  data_pos += 2;
                }
                else if (da_len == 4) {
                  proto_tree_add_item(point_tree, hf_dnp3_al_da_int32, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
                  proto_item_append_text(object_item, ", Value: %d", tvb_get_letohl(tvb, data_pos));
                  data_pos += 4;
                }
                break;
              case AL_DATA_TYPE_FLT:
                da_len = tvb_get_guint8(tvb, data_pos);
                proto_tree_add_item(point_tree, hf_dnp3_al_da_length, tvb, data_pos, 1, ENC_LITTLE_ENDIAN);
                data_pos++;
                if (da_len == 4) {
                  proto_tree_add_item(point_tree, hf_dnp3_al_da_flt, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
                  proto_item_append_text(object_item, ", Value: %g", tvb_get_letohieee_float(tvb, data_pos));
                  data_pos += 4;
                }
                else if (da_len == 8) {
                  proto_tree_add_item(point_tree, hf_dnp3_al_da_dbl, tvb, data_pos, 8, ENC_LITTLE_ENDIAN);
                  proto_item_append_text(object_item, ", Value: %g", tvb_get_letohieee_double(tvb, data_pos));
                  data_pos += 8;
                }
                break;
              case AL_DATA_TYPE_OSTR:
                break;
              case AL_DATA_TYPE_BSTR:
                break;
              case AL_DATA_TYPE_TIME:
                break;
              case AL_DATA_TYPE_UNCD:
                break;
              case AL_DATA_TYPE_U8BS8LIST:
                break;
              case AL_DATA_TYPE_U8BS8EXLIST:
                break;
            }
          }
          offset = data_pos;
        }
        else {

          /* All other objects are handled here, by their variations */
          switch (al_obj)
          {

            /* There is nothing to handle for the default variations */
            case AL_OBJ_BI_ALL:      /* Binary Input Default Variation (Obj:01, Var:Default) */
            case AL_OBJ_BIC_ALL:     /* Binary Input Change Default Variation (Obj:02, Var:Default) */
            case AL_OBJ_BOC_ALL:     /* Binary Output Event Default Variation (Obj:11, Var:Default) */
            case AL_OBJ_2BI_ALL:     /* Double-bit Input Default Variation (Obj:03, Var:Default) */
            case AL_OBJ_2BIC_ALL:    /* Double-bit Input Change Default Variation (Obj:04, Var:Default) */
            case AL_OBJ_CTR_ALL:     /* Binary Counter Default Variation (Obj:20, Var:Default) */
            case AL_OBJ_CTRC_ALL:    /* Binary Counter Change Default Variation (Obj:22 Var:Default) */
            case AL_OBJ_AI_ALL:      /* Analog Input Default Variation (Obj:30, Var:Default) */
            case AL_OBJ_AIC_ALL:     /* Analog Input Change Default Variation (Obj:32 Var:Default) */
            case AL_OBJ_AIDB_ALL:    /* Analog Input Deadband Default Variation (Obj:34, Var:Default) */
            case AL_OBJ_AOC_ALL:     /* Analog Output Event Default Variation (Obj:42 Var:Default) */

              offset = data_pos;
              break;

            /* Bit-based Data objects here */
            case AL_OBJ_BI_1BIT:    /* Single-Bit Binary Input (Obj:01, Var:01) */
            case AL_OBJ_BO:         /* Binary Output (Obj:10, Var:01) */
            case AL_OBJ_CTL_PMASK:  /* Pattern Mask (Obj:12, Var:03) */
            case AL_OBJ_IIN:        /* Internal Indications - IIN (Obj: 80, Var:01) */

              /* Extract the bit from the packed byte */
              al_bi_val = tvb_get_guint8(tvb, data_pos);
              al_bit = (al_bi_val & 1) > 0;
              if (al_obj == AL_OBJ_IIN) {
                /* For an IIN bit, work out the IIN constant value for the bit position to get the name of the bit */
                guint16 iin_bit = 0;
                if (al_ptaddr < 8) {
                  iin_bit = 0x100 << al_ptaddr;
                }
                else {
                  iin_bit = 1 << (al_ptaddr - 8);
                }
                proto_item_append_text(point_item, " (%s), Value: %u",
                                       val_to_str_const(iin_bit, dnp3_al_iin_vals, "Invalid IIN bit"), al_bit);
              }
              else
              {
                if (al_objq_prefix != AL_OBJQL_PREFIX_NI) {
                  /* Each item has an index prefix, in this case bump
                     the bitindex to force the correct offset adjustment */
                  bitindex = 7;
                }
                else {
                  /* Regular packed bits, get the value at the appropriate bit index */
                  al_bit = (al_bi_val & (1 << bitindex)) > 0;
                }
                proto_item_append_text(point_item, ", Value: %u", al_bit);
              }
              proto_tree_add_boolean(point_tree, hf_dnp3_al_bit, tvb, data_pos, 1, al_bit);
              proto_item_set_len(point_item, prefixbytes + 1);

              /* Increment the bit index for next cycle */
              bitindex++;

              /* If we have counted 8 bits or read the last item,
                 reset bit index and move onto the next byte */
              if ((bitindex > 7) || (item_num == (num_items-1)))
              {
                bitindex = 0;
                offset += (prefixbytes + 1);
              }
              break;

            case AL_OBJ_2BI_NF:    /* Double-bit Input No Flags (Obj:03, Var:01) */

              if (bitindex > 3)
              {
                bitindex = 0;
                offset += (prefixbytes + 1);
              }

              /* Extract the Double-bit from the packed byte */
              al_bi_val = tvb_get_guint8(tvb, offset);
              al_2bit = ((al_bi_val >> (bitindex << 1)) & 3);

              proto_item_append_text(point_item, ", Value: %u", al_2bit);
              proto_tree_add_uint(point_tree, hf_dnp3_al_2bit, tvb, offset, 1, al_2bit);
              proto_item_set_len(point_item, prefixbytes + 1);

              /* If we've read the last item, then move the offset past this byte */
              if (item_num == (num_items-1))
              {
                offset += (prefixbytes + 1);
              }

              /* Increment the bit index for next cycle */
              bitindex++;
              break;

            case AL_OBJ_BI_STAT:    /* Binary Input With Status (Obj:01, Var:02) */
            case AL_OBJ_BIC_NOTIME: /* Binary Input Change Without Time (Obj:02, Var:01) */
            case AL_OBJ_BO_STAT:    /* Binary Output Status (Obj:10, Var:02) */
            case AL_OBJ_BOC_NOTIME: /* Binary Output Change Without Time (Obj:11, Var:01) */

              /* Get Point Flags */
              al_ptflags = tvb_get_guint8(tvb, data_pos);

              switch (al_obj) {
                case AL_OBJ_BI_STAT:
                case AL_OBJ_BIC_NOTIME:
                  dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, BIN_IN);
                  break;
                case AL_OBJ_BO_STAT:
                case AL_OBJ_BOC_NOTIME:
                  dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, BIN_OUT);
                  break;
              }
              data_pos += 1;

              al_bit = (al_ptflags & AL_OBJ_BI_FLAG7) > 0;
              proto_item_append_text(point_item, ", Value: %u", al_bit);

              proto_item_set_len(point_item, data_pos - offset);

              offset = data_pos;
              break;

            case AL_OBJ_2BI_STAT:    /* Double-bit Input With Status (Obj:03, Var:02) */
            case AL_OBJ_2BIC_NOTIME: /* Double-bit Input Change Without Time (Obj:04, Var:01) */

              /* Get Point Flags */
              al_ptflags = tvb_get_guint8(tvb, data_pos);
              dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, BIN_IN);
              data_pos += 1;

              al_2bit = (al_ptflags >> 6) & 3;
              proto_item_append_text(point_item, ", Value: %u", al_2bit);
              proto_item_set_len(point_item, data_pos - offset);

              offset = data_pos;
              break;

            case AL_OBJ_BIC_TIME:   /* Binary Input Change w/ Time (Obj:02, Var:02)  */
            case AL_OBJ_BOC_TIME:   /* Binary Output Change w/ Time (Obj:11, Var:02)  */

              /* Get Point Flags */
              al_ptflags = tvb_get_guint8(tvb, data_pos);
              switch (al_obj) {
                case AL_OBJ_BIC_TIME:
                  dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, BIN_IN);
                  break;
                case AL_OBJ_BOC_TIME:
                  dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, BIN_OUT);
                  break;
              }
              data_pos += 1;

              /* Get timestamp */
              dnp3_al_get_timestamp(&al_abstime, tvb, data_pos);
              proto_tree_add_time(point_tree, hf_dnp3_al_timestamp, tvb, data_pos, 6, &al_abstime);
              data_pos += 6;

              al_bit = (al_ptflags & AL_OBJ_BI_FLAG7) >> 7; /* bit shift 1xxxxxxx -> xxxxxxx1 */
              proto_item_append_text(point_item, ", Value: %u, Timestamp: %s",
                                     al_bit, abs_time_to_str(wmem_packet_scope(), &al_abstime, ABSOLUTE_TIME_UTC, FALSE));
              proto_item_set_len(point_item, data_pos - offset);

              offset = data_pos;
              break;

            case AL_OBJ_2BIC_TIME:   /* Double-bit Input Change w/ Time (Obj:04, Var:02)  */

              /* Get Point Flags */
              al_ptflags = tvb_get_guint8(tvb, data_pos);
              dnp3_al_obj_quality(tvb, (offset+prefixbytes), al_ptflags, point_tree, point_item, BIN_IN);
              data_pos += 1;


              /* Get timestamp */
              dnp3_al_get_timestamp(&al_abstime, tvb, data_pos);
              proto_tree_add_time(point_tree, hf_dnp3_al_timestamp, tvb, data_pos, 6, &al_abstime);
              data_pos += 6;

              al_2bit = (al_ptflags >> 6) & 3; /* bit shift 11xxxxxx -> 00000011 */
              proto_item_append_text(point_item, ", Value: %u, Timestamp: %s",
                                     al_2bit, abs_time_to_str(wmem_packet_scope(), &al_abstime, ABSOLUTE_TIME_UTC, FALSE));
              proto_item_set_len(point_item, data_pos - offset);

              offset = data_pos;
              break;

            case AL_OBJ_BIC_RTIME:   /* Binary Input Change w/ Relative Time (Obj:02, Var:03)  */

              /* Get Point Flags */
              al_ptflags = tvb_get_guint8(tvb, data_pos);
              dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, BIN_IN);
              data_pos += 1;

              /* Get relative time in ms, and convert to ns_time */
              al_relms = tvb_get_letohs(tvb, data_pos);
              al_reltime.secs = al_relms / 1000;
              al_reltime.nsecs = (al_relms % 1000) * 1000000;
              /* Now add to CTO time */
              nstime_sum(&al_abstime, al_cto, &al_reltime);
              proto_tree_add_time(point_tree, hf_dnp3_al_rel_timestamp, tvb, data_pos, 2, &al_reltime);
              data_pos += 2;

              al_bit = (al_ptflags & AL_OBJ_BI_FLAG7) >> 7; /* bit shift 1xxxxxxx -> xxxxxxx1 */
              proto_item_append_text(point_item, ", Value: %u, Timestamp: %s",
                                     al_bit, abs_time_to_str(wmem_packet_scope(), &al_abstime, ABSOLUTE_TIME_UTC, FALSE));
              proto_item_set_len(point_item, data_pos - offset);

              offset = data_pos;
              break;

            case AL_OBJ_CTLOP_BLK: /* Control Relay Output Block (Obj:12, Var:01) */
            case AL_OBJ_CTL_PCB: /* Pattern Control Block (Obj:12, Var:02) */
            {
              proto_tree  *tcc_tree;

              /* Add a expand/collapse for TCC */
              al_tcc_code = tvb_get_guint8(tvb, data_pos);
              tcc_tree = proto_tree_add_subtree_format(point_tree, tvb, data_pos, 1,
                          ett_dnp3_al_obj_point_tcc, NULL, "Control Code [0x%02x]",al_tcc_code);

              /* Add the Control Code to the Point number list for quick visual reference as to the operation */
              proto_item_append_text(point_item, " [%s]", val_to_str_const((al_tcc_code & AL_OBJCTLC_CODE),
                                                                           dnp3_al_ctlc_code_vals,
                                                                           "Invalid Operation"));

              /* Add Trip/Close qualifier (if applicable) to previously appended quick visual reference */
              proto_item_append_text(point_item, " [%s]", val_to_str_const((al_tcc_code & AL_OBJCTLC_TC) >> 6,
                                                                           dnp3_al_ctlc_tc_vals,
                                                                           "Invalid Qualifier"));



              /* Control Code 'Operation Type' */
              proto_tree_add_item(tcc_tree, hf_dnp3_ctlobj_code_c, tvb, data_pos, 1, ENC_LITTLE_ENDIAN);

              /* Control Code Misc Values */
              proto_tree_add_item(tcc_tree, hf_dnp3_ctlobj_code_m, tvb, data_pos, 1, ENC_LITTLE_ENDIAN);

              /* Control Code 'Trip Close Code' */
              proto_tree_add_item(tcc_tree, hf_dnp3_ctlobj_code_tc, tvb, data_pos, 1, ENC_LITTLE_ENDIAN);
              data_pos += 1;

              /* Get "Count" Field */
              proto_tree_add_item(point_tree, hf_dnp3_al_count, tvb, data_pos, 1, ENC_LITTLE_ENDIAN);
              data_pos += 1;

              /* Get "On Time" Field */
              proto_tree_add_item(point_tree, hf_dnp3_al_on_time, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
              data_pos += 4;

              /* Get "Off Time" Field */
              proto_tree_add_item(point_tree, hf_dnp3_al_off_time, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
              data_pos += 4;

              /* Get "Control Status" Field */
              proto_tree_add_item(point_tree, hf_dnp3_al_ctrlstatus, tvb, data_pos, 1, ENC_LITTLE_ENDIAN);
              data_pos += 1;

              proto_item_set_len(point_item, data_pos - offset);

              offset = data_pos;
              break;
            }

            case AL_OBJ_BOE_NOTIME: /* Binary Command Event (Obj:13, Var:01) */
            case AL_OBJ_BOE_TIME:   /* Binary Command Event with time (Obj:13, Var:02) */
            case AL_OBJ_AOC_32EVNT:   /* 32-bit Analog Command Event (Obj:43, Var:01) */
            case AL_OBJ_AOC_16EVNT:   /* 16-bit Analog Command Event (Obj:43, Var:02) */
            case AL_OBJ_AOC_32EVTT:   /* 32-bit Analog Command Event with time (Obj:43, Var:03) */
            case AL_OBJ_AOC_16EVTT:   /* 16-bit Analog Command Event with time (Obj:43, Var:04) */
            case AL_OBJ_AOC_FLTEVNT:   /* 32-bit Floating Point Analog Command Event (Obj:43, Var:05) */
            case AL_OBJ_AOC_DBLEVNT:   /* 64-bit Floating Point Analog Command Event (Obj:43, Var:06) */
            case AL_OBJ_AOC_FLTEVTT:   /* 32-bit Floating Point Analog Command Event with time (Obj:43, Var:07) */
            case AL_OBJ_AOC_DBLEVTT:   /* 64-bit Floating Point Analog Command Event with time (Obj:43, Var:08) */
            {
              /* Get the status code */
              al_ctlobj_stat = tvb_get_guint8(tvb, data_pos) & AL_OBJCTL_STATUS_MASK;
              ctl_status_str = val_to_str_ext(al_ctlobj_stat, &dnp3_al_ctl_status_vals_ext, "Invalid Status (0x%02x)");
              proto_item_append_text(point_item, " [Status: %s (0x%02x)]", ctl_status_str, al_ctlobj_stat);
              proto_tree_add_item(point_tree, hf_dnp3_al_ctrlstatus, tvb, data_pos, 1, ENC_LITTLE_ENDIAN);

              /* Get the command value */
              switch(al_obj)
              {
                case AL_OBJ_BOE_NOTIME:
                case AL_OBJ_BOE_TIME:
                  proto_tree_add_item(point_tree, hf_dnp3_bocs_bit, tvb, data_pos, 1, ENC_LITTLE_ENDIAN);
                  data_pos += 1;
                  break;
                case AL_OBJ_AOC_32EVNT:
                case AL_OBJ_AOC_32EVTT:
                  data_pos += 1; /* Step past status */
                  al_val_int32 = tvb_get_letohl(tvb, data_pos);
                  proto_item_append_text(point_item, ", Value: %d", al_val_int32);
                  proto_tree_add_item(point_tree, hf_dnp3_al_anaout32, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
                  break;
                case AL_OBJ_AOC_16EVNT:
                case AL_OBJ_AOC_16EVTT:
                  data_pos += 1; /* Step past status */
                  al_val_int16 = tvb_get_letohs(tvb, data_pos);
                  proto_item_append_text(point_item, ", Value: %d", al_val_int16);
                  proto_tree_add_item(point_tree, hf_dnp3_al_anaout16, tvb, data_pos, 2, ENC_LITTLE_ENDIAN);
                  data_pos += 2;
                  break;
                case AL_OBJ_AOC_FLTEVNT:
                case AL_OBJ_AOC_FLTEVTT:
                  data_pos += 1; /* Step past status */
                  al_valflt = tvb_get_letohieee_float(tvb, data_pos);
                  proto_item_append_text(point_item, ", Value: %g", al_valflt);
                  proto_tree_add_item(point_tree, hf_dnp3_al_anaoutflt, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
                  data_pos += 4;
                  break;
                case AL_OBJ_AOC_DBLEVNT:
                case AL_OBJ_AOC_DBLEVTT:
                  data_pos += 1; /* Step past status */
                  al_valdbl = tvb_get_letohieee_double(tvb, data_pos);
                  proto_item_append_text(point_item, ", Value: %g", al_valdbl);
                  proto_tree_add_item(point_tree, hf_dnp3_al_anaoutdbl, tvb, data_pos, 8, ENC_LITTLE_ENDIAN);
                  data_pos += 8;
                  break;
              }

              /* Get the timestamp */
              switch(al_obj)
              {
                case AL_OBJ_BOE_TIME:   /* Binary Command Event with time (Obj:13, Var:02) */
                case AL_OBJ_AOC_32EVTT:   /* 32-bit Analog Command Event with time (Obj:43, Var:03) */
                case AL_OBJ_AOC_16EVTT:   /* 16-bit Analog Command Event with time (Obj:43, Var:04) */
                case AL_OBJ_AOC_FLTEVTT:   /* 32-bit Floating Point Analog Command Event with time (Obj:43, Var:07) */
                case AL_OBJ_AOC_DBLEVTT:   /* 64-bit Floating Point Analog Command Event with time (Obj:43, Var:08) */
                  dnp3_al_get_timestamp(&al_abstime, tvb, data_pos);
                  proto_item_append_text(point_item, ", Timestamp: %s", abs_time_to_str(wmem_packet_scope(), &al_abstime, ABSOLUTE_TIME_UTC, FALSE));
                  proto_tree_add_time(point_tree, hf_dnp3_al_timestamp, tvb, data_pos, 6, &al_abstime);
                  data_pos += 6;
                break;
              }

              proto_item_set_len(point_item, data_pos - offset);
              offset = data_pos;
              break;
            }

            case AL_OBJ_AO_32OPB:   /* 32-Bit Analog Output Block (Obj:41, Var:01) */
            case AL_OBJ_AO_16OPB:   /* 16-Bit Analog Output Block (Obj:41, Var:02) */
            case AL_OBJ_AO_FLTOPB:  /* 32-Bit Floating Point Output Block (Obj:41, Var:03) */
            case AL_OBJ_AO_DBLOPB:  /* 64-Bit Floating Point Output Block (Obj:41, Var:04) */

              switch (al_obj)
              {
                case AL_OBJ_AO_32OPB:
                  al_val_int32 = tvb_get_letohl(tvb, data_pos);
                  proto_item_append_text(point_item, ", Value: %d", al_val_int32);
                  proto_tree_add_item(point_tree, hf_dnp3_al_anaout32, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
                  data_pos += 4;
                  break;
                case AL_OBJ_AO_16OPB:
                  al_val_int16 = tvb_get_letohs(tvb, data_pos);
                  proto_item_append_text(point_item, ", Value: %d", al_val_int16);
                  proto_tree_add_item(point_tree, hf_dnp3_al_anaout16, tvb, data_pos, 2, ENC_LITTLE_ENDIAN);
                  data_pos += 2;
                  break;
                case AL_OBJ_AO_FLTOPB:
                  al_valflt = tvb_get_letohieee_float(tvb, data_pos);
                  proto_item_append_text(point_item, ", Value: %g", al_valflt);
                  proto_tree_add_item(point_tree, hf_dnp3_al_anaoutflt, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
                  data_pos += 4;
                  break;
                case AL_OBJ_AO_DBLOPB:
                  al_valdbl = tvb_get_letohieee_double(tvb, data_pos);
                  proto_item_append_text(point_item, ", Value: %g", al_valdbl);
                  proto_tree_add_item(point_tree, hf_dnp3_al_anaoutdbl, tvb, data_pos, 8, ENC_LITTLE_ENDIAN);
                  data_pos += 8;
                  break;
              }

              /* Get control status */
              al_ctlobj_stat = tvb_get_guint8(tvb, data_pos) & AL_OBJCTL_STATUS_MASK;
              ctl_status_str = val_to_str_ext(al_ctlobj_stat, &dnp3_al_ctl_status_vals_ext, "Invalid Status (0x%02x)");
              proto_item_append_text(point_item, " [Status: %s (0x%02x)]", ctl_status_str, al_ctlobj_stat);
              proto_tree_add_item(point_tree, hf_dnp3_al_ctrlstatus, tvb, data_pos, 1, ENC_LITTLE_ENDIAN);
              data_pos += 1;

              proto_item_set_len(point_item, data_pos - offset);

              offset = data_pos;
              break;

            case AL_OBJ_CTR_32:     /* 32-Bit Binary Counter (Obj:20, Var:01) */
            case AL_OBJ_CTR_16:     /* 16-Bit Binary Counter (Obj:20, Var:02) */
            case AL_OBJ_DCTR_32:    /* 32-Bit Binary Delta Counter (Obj:20, Var:03) */
            case AL_OBJ_DCTR_16:    /* 16-Bit Binary Delta Counter (Obj:20, Var:04) */
            case AL_OBJ_CTR_32NF:   /* 32-Bit Binary Counter Without Flag (Obj:20, Var:05) */
            case AL_OBJ_CTR_16NF:   /* 16-Bit Binary Counter Without Flag (Obj:20, Var:06) */
            case AL_OBJ_DCTR_32NF:  /* 32-Bit Binary Delta Counter Without Flag (Obj:20, Var:07) */
            case AL_OBJ_DCTR_16NF:  /* 16-Bit Binary Delta Counter Without Flag (Obj:20, Var:08) */
            case AL_OBJ_FCTR_32:    /* 32-Bit Frozen Counter (Obj:21, Var:01) */
            case AL_OBJ_FCTR_16:    /* 16-Bit Frozen Counter (Obj:21, Var:02) */
            case AL_OBJ_FDCTR_32:   /* 21 03 32-Bit Frozen Delta Counter */
            case AL_OBJ_FDCTR_16:   /* 21 04 16-Bit Frozen Delta Counter */
            case AL_OBJ_FCTR_32T:   /* 32-Bit Frozen Counter w/ Time of Freeze (Obj:21 Var:05 ) */
            case AL_OBJ_FCTR_16T:   /* 16-Bit Frozen Counter w/ Time of Freeze (Obj:21 Var:06) */
            case AL_OBJ_FDCTR_32T:  /* 32-Bit Frozen Delta Counter w/ Time of Freeze (Obj:21 Var:07) */
            case AL_OBJ_FDCTR_16T:  /* 16-Bit Frozen Delta Counter w/ Time of Freeze (Obj:21 Var:08) */
            case AL_OBJ_FCTR_32NF:  /* 32-Bit Frozen Counter Without Flag (Obj:21 Var:09) */
            case AL_OBJ_FCTR_16NF:  /* 16-Bit Frozen Counter Without Flag (Obj:21 Var:10) */
            case AL_OBJ_FDCTR_32NF: /* 32-Bit Frozen Delta Counter Without Flag (Obj:21 Var:11) */
            case AL_OBJ_FDCTR_16NF: /* 16-Bit Frozen Delta Counter Without Flag (Obj:21 Var:12) */
            case AL_OBJ_CTRC_32:    /* 32-Bit Counter Change Event w/o Time (Obj:22, Var:01) */
            case AL_OBJ_CTRC_16:    /* 16-Bit Counter Change Event w/o Time (Obj:22, Var:02) */
            case AL_OBJ_DCTRC_32:   /* 32-Bit Delta Counter Change Event w/o Time (Obj:22, Var:03) */
            case AL_OBJ_DCTRC_16:   /* 16-Bit Delta Counter Change Event w/o Time (Obj:22, Var:04) */
            case AL_OBJ_CTRC_32T:   /* 32-Bit Counter Change Event with Time (Obj:22, Var:05) */
            case AL_OBJ_CTRC_16T:   /* 16-Bit Counter Change Event with Time (Obj:22, Var:06) */
            case AL_OBJ_DCTRC_32T:  /* 32-Bit Delta Counter Change Event with Time (Obj:22, Var:07) */
            case AL_OBJ_DCTRC_16T:  /* 16-Bit Delta Counter Change Event with Time (Obj:22, Var:08) */
            case AL_OBJ_FCTRC_32:   /* 32-Bit Frozen Counter Change Event (Obj:23 Var:01) */
            case AL_OBJ_FCTRC_16:   /* 16-Bit Frozen Counter Change Event (Obj:23 Var:02) */
            case AL_OBJ_FDCTRC_32:  /* 32-Bit Frozen Delta Counter Change Event (Obj:23 Var:03) */
            case AL_OBJ_FDCTRC_16:  /* 16-Bit Frozen Delta Counter Change Event (Obj:23 Var:04) */
            case AL_OBJ_FCTRC_32T:  /* 32-Bit Frozen Counter Change Event w/ Time of Freeze (Obj:23 Var:05) */
            case AL_OBJ_FCTRC_16T:  /* 16-Bit Frozen Counter Change Event w/ Time of Freeze (Obj:23 Var:06) */
            case AL_OBJ_FDCTRC_32T: /* 32-Bit Frozen Delta Counter Change Event w/ Time of Freeze (Obj:23 Var:07) */
            case AL_OBJ_FDCTRC_16T: /* 16-Bit Frozen Delta Counter Change Event w/ Time of Freeze (Obj:23 Var:08) */

              /* Get Point Flags for those types that have them, it's easier to block out those that don't have flags */
              switch (al_obj)
              {
                case AL_OBJ_CTR_32NF:
                case AL_OBJ_CTR_16NF:
                case AL_OBJ_DCTR_32NF:
                case AL_OBJ_DCTR_16NF:
                case AL_OBJ_FCTR_32NF:
                case AL_OBJ_FCTR_16NF:
                case AL_OBJ_FDCTR_32NF:
                case AL_OBJ_FDCTR_16NF:
                  break;

                default:
                  al_ptflags = tvb_get_guint8(tvb, data_pos);
                  dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, COUNTER);
                  data_pos += 1;
                  break;
              }

              /* Get Counter values */
              switch (al_obj)
              {
                case AL_OBJ_CTR_32:
                case AL_OBJ_DCTR_32:
                case AL_OBJ_CTR_32NF:
                case AL_OBJ_DCTR_32NF:
                case AL_OBJ_FCTR_32:
                case AL_OBJ_FDCTR_32:
                case AL_OBJ_FCTR_32T:
                case AL_OBJ_FDCTR_32T:
                case AL_OBJ_FCTR_32NF:
                case AL_OBJ_FDCTR_32NF:
                case AL_OBJ_CTRC_32:
                case AL_OBJ_DCTRC_32:
                case AL_OBJ_CTRC_32T:
                case AL_OBJ_DCTRC_32T:
                case AL_OBJ_FCTRC_32:
                case AL_OBJ_FDCTRC_32:
                case AL_OBJ_FCTRC_32T:
                case AL_OBJ_FDCTRC_32T:

                  al_val_uint32 = tvb_get_letohl(tvb, data_pos);
                  proto_item_append_text(point_item, ", Count: %u", al_val_uint32);
                  proto_tree_add_item(point_tree, hf_dnp3_al_cnt32, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
                  data_pos += 4;
                  break;

                case AL_OBJ_CTR_16:
                case AL_OBJ_DCTR_16:
                case AL_OBJ_CTR_16NF:
                case AL_OBJ_DCTR_16NF:
                case AL_OBJ_FCTR_16:
                case AL_OBJ_FDCTR_16:
                case AL_OBJ_FCTR_16T:
                case AL_OBJ_FDCTR_16T:
                case AL_OBJ_FCTR_16NF:
                case AL_OBJ_FDCTR_16NF:
                case AL_OBJ_CTRC_16:
                case AL_OBJ_DCTRC_16:
                case AL_OBJ_CTRC_16T:
                case AL_OBJ_DCTRC_16T:
                case AL_OBJ_FCTRC_16:
                case AL_OBJ_FDCTRC_16:
                case AL_OBJ_FCTRC_16T:
                case AL_OBJ_FDCTRC_16T:

                  al_val_uint16 = tvb_get_letohs(tvb, data_pos);
                  proto_item_append_text(point_item, ", Count: %u", al_val_uint16);
                  proto_tree_add_item(point_tree, hf_dnp3_al_cnt16, tvb, data_pos, 2, ENC_LITTLE_ENDIAN);
                  data_pos += 2;
                  break;
              }

              /* Get the time for those points that have it */
              switch (al_obj)
              {
                case AL_OBJ_FCTR_32T:
                case AL_OBJ_FCTR_16T:
                case AL_OBJ_FDCTR_32T:
                case AL_OBJ_FDCTR_16T:
                case AL_OBJ_CTRC_32T:
                case AL_OBJ_CTRC_16T:
                case AL_OBJ_DCTRC_32T:
                case AL_OBJ_DCTRC_16T:
                case AL_OBJ_FCTRC_32T:
                case AL_OBJ_FCTRC_16T:
                case AL_OBJ_FDCTRC_32T:
                case AL_OBJ_FDCTRC_16T:
                  dnp3_al_get_timestamp(&al_abstime, tvb, data_pos);
                  proto_item_append_text(point_item, ", Timestamp: %s", abs_time_to_str(wmem_packet_scope(), &al_abstime, ABSOLUTE_TIME_UTC, FALSE));
                  proto_tree_add_time(point_tree, hf_dnp3_al_timestamp, tvb, data_pos, 6, &al_abstime);
                  data_pos += 6;
                  break;
              }

              proto_item_set_len(point_item, data_pos - offset);
              offset = data_pos;
              break;

            case AL_OBJ_AI_32:        /* 32-Bit Analog Input (Obj:30, Var:01) */
            case AL_OBJ_AI_16:        /* 16-Bit Analog Input (Obj:30, Var:02) */
            case AL_OBJ_AI_32NF:      /* 32-Bit Analog Input Without Flag (Obj:30, Var:03) */
            case AL_OBJ_AI_16NF:      /* 16-Bit Analog Input Without Flag (Obj:30, Var:04) */
            case AL_OBJ_AI_FLT:       /* 32-Bit Floating Point Input (Obj:30, Var:05) */
            case AL_OBJ_AI_DBL:       /* 64-Bit Floating Point Input (Obj:30, Var:06) */
            case AL_OBJ_AIF_FLT:      /* 32-Bit Frozen Floating Point Input (Obj:31, Var:07) */
            case AL_OBJ_AIF_DBL:      /* 64-Bit Frozen Floating Point Input (Obj:31, Var:08) */
            case AL_OBJ_AIC_32NT:     /* 32-Bit Analog Change Event w/o Time (Obj:32, Var:01) */
            case AL_OBJ_AIC_16NT:     /* 16-Bit Analog Change Event w/o Time (Obj:32, Var:02) */
            case AL_OBJ_AIC_32T:      /* 32-Bit Analog Change Event with Time (Obj:32, Var:03) */
            case AL_OBJ_AIC_16T:      /* 16-Bit Analog Change Event with Time (Obj:32, Var:04) */
            case AL_OBJ_AIC_FLTNT:    /* 32-Bit Floating Point Change Event w/o Time (Obj:32, Var:05) */
            case AL_OBJ_AIC_DBLNT:    /* 64-Bit Floating Point Change Event w/o Time (Obj:32, Var:06) */
            case AL_OBJ_AIC_FLTT:     /* 32-Bit Floating Point Change Event w/ Time (Obj:32, Var:07) */
            case AL_OBJ_AIC_DBLT:     /* 64-Bit Floating Point Change Event w/ Time (Obj:32, Var:08) */
            case AL_OBJ_AIFC_FLTNT:   /* 32-Bit Floating Point Frozen Change Event w/o Time (Obj:33, Var:05) */
            case AL_OBJ_AIFC_DBLNT:   /* 64-Bit Floating Point Frozen Change Event w/o Time (Obj:33, Var:06) */
            case AL_OBJ_AIFC_FLTT:    /* 32-Bit Floating Point Frozen Change Event w/ Time (Obj:33, Var:07) */
            case AL_OBJ_AIFC_DBLT:    /* 64-Bit Floating Point Frozen Change Event w/ Time (Obj:33, Var:08) */
            case AL_OBJ_AIDB_16:      /* 16-Bit Analog Input Deadband (Obj:34, Var:01) */
            case AL_OBJ_AIDB_32:      /* 32-Bit Analog Input Deadband (Obj:34, Var:02) */
            case AL_OBJ_AIDB_FLT:     /* 32-Bit Floating Point Analog Input Deadband (Obj:34, Var:03) */

              /* Get Point Flags for those types that have them */
              switch (al_obj)
              {
                case AL_OBJ_AI_32NF:
                case AL_OBJ_AI_16NF:
                case AL_OBJ_AIDB_16:
                case AL_OBJ_AIDB_32:
                case AL_OBJ_AIDB_FLT:
                  break;

                default:
                  al_ptflags = tvb_get_guint8(tvb, data_pos);
                  dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, ANA_IN);
                  data_pos += 1;
                  break;
              }

              switch (al_obj)
              {
                case AL_OBJ_AI_32:
                case AL_OBJ_AI_32NF:
                case AL_OBJ_AIC_32NT:
                case AL_OBJ_AIC_32T:
                case AL_OBJ_AIDB_32:

                  al_val_int32 = tvb_get_letohl(tvb, data_pos);
                  proto_item_append_text(point_item, ", Value: %d", al_val_int32);
                  proto_tree_add_item(point_tree, hf_dnp3_al_ana32, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
                  data_pos += 4;
                  break;

                case AL_OBJ_AI_16:
                case AL_OBJ_AI_16NF:
                case AL_OBJ_AIC_16NT:
                case AL_OBJ_AIC_16T:
                case AL_OBJ_AIDB_16:

                  al_val_int16 = tvb_get_letohs(tvb, data_pos);
                  proto_item_append_text(point_item, ", Value: %d", al_val_int16);
                  proto_tree_add_item(point_tree, hf_dnp3_al_ana16, tvb, data_pos, 2, ENC_LITTLE_ENDIAN);
                  data_pos += 2;
                  break;

                case AL_OBJ_AI_FLT:
                case AL_OBJ_AIF_FLT:
                case AL_OBJ_AIC_FLTNT:
                case AL_OBJ_AIC_FLTT:
                case AL_OBJ_AIFC_FLTNT:
                case AL_OBJ_AIFC_FLTT:
                case AL_OBJ_AIDB_FLT:

                  al_valflt = tvb_get_letohieee_float(tvb, data_pos);
                  proto_item_append_text(point_item, ", Value: %g", al_valflt);
                  proto_tree_add_item(point_tree, hf_dnp3_al_anaflt, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
                  data_pos += 4;
                  break;

                case AL_OBJ_AI_DBL:
                case AL_OBJ_AIF_DBL:
                case AL_OBJ_AIC_DBLNT:
                case AL_OBJ_AIC_DBLT:
                case AL_OBJ_AIFC_DBLNT:
                case AL_OBJ_AIFC_DBLT:

                  al_valdbl = tvb_get_letohieee_double(tvb, data_pos);
                  proto_item_append_text(point_item, ", Value: %g", al_valdbl);
                  proto_tree_add_item(point_tree, hf_dnp3_al_anadbl, tvb, data_pos, 8, ENC_LITTLE_ENDIAN);
                  data_pos += 8;
                  break;
              }

              /* Get timestamp */
              switch (al_obj)
              {
                case AL_OBJ_AIC_32T:
                case AL_OBJ_AIC_16T:
                case AL_OBJ_AIC_FLTT:
                case AL_OBJ_AIC_DBLT:
                case AL_OBJ_AIFC_FLTT:
                case AL_OBJ_AIFC_DBLT:
                  dnp3_al_get_timestamp(&al_abstime, tvb, data_pos);
                  proto_item_append_text(point_item, ", Timestamp: %s", abs_time_to_str(wmem_packet_scope(), &al_abstime, ABSOLUTE_TIME_UTC, FALSE));
                  proto_tree_add_time(point_tree, hf_dnp3_al_timestamp, tvb, data_pos, 6, &al_abstime);
                  data_pos += 6;
                  break;
              }

              proto_item_set_len(point_item, data_pos - offset);

              offset = data_pos;
              break;

            case AL_OBJ_AO_32:     /* 32-Bit Analog Output Status (Obj:40, Var:01) */
            case AL_OBJ_AO_16:     /* 16-Bit Analog Output Status (Obj:40, Var:02) */
            case AL_OBJ_AO_FLT:    /* 32-Bit Floating Point Output Status (Obj:40, Var:03) */
            case AL_OBJ_AO_DBL:    /* 64-Bit Floating Point Output Status (Obj:40, Var:04) */
            case AL_OBJ_AOC_32NT:  /* 32-Bit Analog Output Event w/o Time (Obj:42, Var:01) */
            case AL_OBJ_AOC_16NT:  /* 16-Bit Analog Output Event w/o Time (Obj:42, Var:02) */
            case AL_OBJ_AOC_32T:   /* 32-Bit Analog Output Event with Time (Obj:42, Var:03) */
            case AL_OBJ_AOC_16T:   /* 16-Bit Analog Output Event with Time (Obj:42, Var:04) */
            case AL_OBJ_AOC_FLTNT: /* 32-Bit Floating Point Output Event w/o Time (Obj:42, Var:05) */
            case AL_OBJ_AOC_DBLNT: /* 64-Bit Floating Point Output Event w/o Time (Obj:42, Var:06) */
            case AL_OBJ_AOC_FLTT:  /* 32-Bit Floating Point Output Event w/ Time (Obj:42, Var:07) */
            case AL_OBJ_AOC_DBLT:  /* 64-Bit Floating Point Output Event w/ Time (Obj:42, Var:08) */

              /* Get Point Flags */
              al_ptflags = tvb_get_guint8(tvb, data_pos);
              dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, ANA_OUT);
              data_pos += 1;

              switch (al_obj)
              {
                case AL_OBJ_AO_32:     /* 32-Bit Analog Output Status (Obj:40, Var:01) */
                case AL_OBJ_AOC_32NT:  /* 32-Bit Analog Output Event w/o Time (Obj:42, Var:01) */
                case AL_OBJ_AOC_32T:   /* 32-Bit Analog Output Event with Time (Obj:42, Var:03) */

                  al_val_int32 = tvb_get_letohl(tvb, data_pos);
                  proto_item_append_text(point_item, ", Value: %d", al_val_int32);
                  proto_tree_add_item(point_tree, hf_dnp3_al_anaout32, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
                  data_pos += 4;
                  break;

                case AL_OBJ_AO_16:     /* 16-Bit Analog Output Status (Obj:40, Var:02) */
                case AL_OBJ_AOC_16NT:  /* 16-Bit Analog Output Event w/o Time (Obj:42, Var:02) */
                case AL_OBJ_AOC_16T:   /* 16-Bit Analog Output Event with Time (Obj:42, Var:04) */

                  al_val_int16 = tvb_get_letohs(tvb, data_pos);
                  proto_item_append_text(point_item, ", Value: %d", al_val_int16);
                  proto_tree_add_item(point_tree, hf_dnp3_al_anaout16, tvb, data_pos, 2, ENC_LITTLE_ENDIAN);
                  data_pos += 2;
                  break;

                case AL_OBJ_AO_FLT:     /* 32-Bit Floating Point Output Status (Obj:40, Var:03) */
                case AL_OBJ_AOC_FLTNT:  /* 32-Bit Floating Point Output Event w/o Time (Obj:42, Var:05) */
                case AL_OBJ_AOC_FLTT:   /* 32-Bit Floating Point Output Event w/ Time (Obj:42, Var:07) */

                  al_valflt = tvb_get_letohieee_float(tvb, data_pos);
                  proto_item_append_text(point_item, ", Value: %g", al_valflt);
                  proto_tree_add_item(point_tree, hf_dnp3_al_anaoutflt, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
                  data_pos += 4;
                  break;

                case AL_OBJ_AO_DBL:     /* 64-Bit Floating Point Output Status (Obj:40, Var:04) */
                case AL_OBJ_AOC_DBLNT:  /* 64-Bit Floating Point Output Event w/o Time (Obj:42, Var:06) */
                case AL_OBJ_AOC_DBLT:   /* 64-Bit Floating Point Output Event w/ Time (Obj:42, Var:08) */

                  al_valdbl = tvb_get_letohieee_double(tvb, data_pos);
                  proto_item_append_text(point_item, ", Value: %g", al_valdbl);
                  proto_tree_add_item(point_tree, hf_dnp3_al_anaoutdbl, tvb, data_pos, 8, ENC_LITTLE_ENDIAN);
                  data_pos += 8;
                  break;
              }

              /* Get timestamp */
              switch (al_obj)
              {
                case AL_OBJ_AOC_32T:
                case AL_OBJ_AOC_16T:
                case AL_OBJ_AOC_FLTT:
                case AL_OBJ_AOC_DBLT:
                  dnp3_al_get_timestamp(&al_abstime, tvb, data_pos);
                  proto_item_append_text(point_item, ", Timestamp: %s", abs_time_to_str(wmem_packet_scope(), &al_abstime, ABSOLUTE_TIME_UTC, FALSE));
                  proto_tree_add_time(point_tree, hf_dnp3_al_timestamp, tvb, data_pos, 6, &al_abstime);
                  data_pos += 6;
                  break;
              }

              proto_item_set_len(point_item, data_pos - offset);
              offset = data_pos;
              break;

            case AL_OBJ_TD:    /* Time and Date (Obj:50, Var:01) */
            case AL_OBJ_TDR:   /* Time and Date at Last Recorded Time (Obj:50, Var:03) */
            case AL_OBJ_TDCTO: /* Time and Date CTO (Obj:51, Var:01) */

              dnp3_al_get_timestamp(&al_abstime, tvb, data_pos);
              proto_tree_add_time(object_tree, hf_dnp3_al_timestamp, tvb, data_pos, 6, &al_abstime);
              data_pos += 6;
              proto_item_set_len(point_item, data_pos - offset);

              if (al_obj == AL_OBJ_TDCTO) {
                /* Copy the time object to the CTO for any other relative time objects in this response */
                nstime_copy(al_cto, &al_abstime);
              }

              offset = data_pos;
              break;

            case AL_OBJ_TDELAYF: /* Time Delay - Fine (Obj:52, Var:02) */

              proto_tree_add_item(object_tree, hf_dnp3_al_time_delay, tvb, data_pos, 2, ENC_LITTLE_ENDIAN);
              data_pos += 2;
              proto_item_set_len(point_item, data_pos - offset);

              offset = data_pos;
              break;

            case AL_OBJ_CLASS0:  /* Class Data Objects */
            case AL_OBJ_CLASS1:
            case AL_OBJ_CLASS2:
            case AL_OBJ_CLASS3:

              /* No data here */
              offset = data_pos;
              break;

            case AL_OBJ_FILE_CMD: /* File Control - File Command (Obj:70, Var:03) */
              /* File name offset and length */
              proto_tree_add_item(point_tree, hf_dnp3_al_file_string_offset, tvb, data_pos, 2, ENC_LITTLE_ENDIAN);
              data_pos += 2;
              al_filename_len = tvb_get_letohs(tvb, data_pos);
              proto_tree_add_item(point_tree, hf_dnp3_al_file_string_length, tvb, data_pos, 2, ENC_LITTLE_ENDIAN);
              data_pos += 2;

              /* Grab the mode as it determines if some of the following fields are relevant */
              al_file_ctrl_mode = tvb_get_letohs(tvb, data_pos + 16);

              /* Creation Time */
              if (al_file_ctrl_mode == AL_OBJ_FILE_MODE_WRITE) {
                dnp3_al_get_timestamp(&al_abstime, tvb, data_pos);
                proto_tree_add_time(point_tree, hf_dnp3_al_timestamp, tvb, data_pos, 6, &al_abstime);
              }
              data_pos += 6;

              /* Perms */
              if (al_file_ctrl_mode == AL_OBJ_FILE_MODE_WRITE) {
                proto_item *perms_item;
                proto_tree *perms_tree;

                perms_item = proto_tree_add_item(point_tree, hf_dnp3_al_file_perms, tvb, offset, 2, ENC_LITTLE_ENDIAN);

                perms_tree = proto_item_add_subtree(perms_item, ett_dnp3_al_obj_point_perms);
                proto_tree_add_item(perms_tree, hf_dnp3_al_file_perms_read_owner,  tvb, offset, 2, ENC_LITTLE_ENDIAN);
                proto_tree_add_item(perms_tree, hf_dnp3_al_file_perms_write_owner, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                proto_tree_add_item(perms_tree, hf_dnp3_al_file_perms_exec_owner,  tvb, offset, 2, ENC_LITTLE_ENDIAN);
                proto_tree_add_item(perms_tree, hf_dnp3_al_file_perms_read_group,  tvb, offset, 2, ENC_LITTLE_ENDIAN);
                proto_tree_add_item(perms_tree, hf_dnp3_al_file_perms_write_group, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                proto_tree_add_item(perms_tree, hf_dnp3_al_file_perms_exec_group,  tvb, offset, 2, ENC_LITTLE_ENDIAN);
                proto_tree_add_item(perms_tree, hf_dnp3_al_file_perms_read_world,  tvb, offset, 2, ENC_LITTLE_ENDIAN);
                proto_tree_add_item(perms_tree, hf_dnp3_al_file_perms_write_world, tvb, offset, 2, ENC_LITTLE_ENDIAN);
                proto_tree_add_item(perms_tree, hf_dnp3_al_file_perms_exec_world,  tvb, offset, 2, ENC_LITTLE_ENDIAN);
              }
              data_pos += 2;

              /* Auth Key */
              proto_tree_add_item(point_tree, hf_dnp3_al_file_auth, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
              data_pos += 4;

              /* File Size */
              if (al_file_ctrl_mode == AL_OBJ_FILE_MODE_WRITE || al_file_ctrl_mode == AL_OBJ_FILE_MODE_APPEND) {
                proto_tree_add_item(point_tree, hf_dnp3_al_file_size, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
              }
              data_pos += 4;

              /* Mode */
              proto_tree_add_item(point_tree, hf_dnp3_al_file_mode, tvb, data_pos, 2, ENC_LITTLE_ENDIAN);
              data_pos += 2;

              /* Max Block Size */
              proto_tree_add_item(point_tree, hf_dnp3_al_file_maxblk, tvb, data_pos, 2, ENC_LITTLE_ENDIAN);
              data_pos += 2;

              /* Request ID */
              proto_tree_add_item(point_tree, hf_dnp3_al_file_reqID, tvb, data_pos, 2, ENC_LITTLE_ENDIAN);
              data_pos += 2;

              /* Filename */
              if (al_filename_len > 0) {
                proto_tree_add_item(point_tree, hf_dnp3_al_file_name, tvb, data_pos, al_filename_len, ENC_ASCII|ENC_NA);
              }
              data_pos += al_filename_len;
              proto_item_set_len(point_item, data_pos - offset);

              offset = data_pos;
              break;

            case AL_OBJ_FILE_STAT: /* File Control - File Status (Obj:70, Var:04) */

              /* File Handle */
              proto_tree_add_item(point_tree, hf_dnp3_al_file_handle, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
              data_pos += 4;

              /* File Size */
              proto_tree_add_item(point_tree, hf_dnp3_al_file_size,   tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
              data_pos += 4;

              /* Max Block Size */
              proto_tree_add_item(point_tree, hf_dnp3_al_file_maxblk, tvb, data_pos, 2, ENC_LITTLE_ENDIAN);
              data_pos += 2;

              /* Request ID */
              proto_tree_add_item(point_tree, hf_dnp3_al_file_reqID,  tvb, data_pos, 2, ENC_LITTLE_ENDIAN);
              data_pos += 2;

              /* Status code */
              proto_tree_add_item(point_tree, hf_dnp3_al_file_status, tvb, data_pos, 1, ENC_LITTLE_ENDIAN);
              data_pos += 1;

              /* Optional text */
              file_data_size = al_ptaddr - (data_pos - offset - prefixbytes);
              if ((file_data_size) > 0) {
                proto_tree_add_item(point_tree, hf_dnp3_al_file_data, tvb, data_pos, file_data_size, ENC_NA);
                data_pos += file_data_size;
              }

              proto_item_set_len(point_item, data_pos - offset);

              offset = data_pos;
              break;

            case AL_OBJ_FILE_TRANS: /* File Control - File Transport (Obj:70, Var:05) */

              /* File Handle */
              proto_tree_add_item(point_tree, hf_dnp3_al_file_handle, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
              data_pos += 4;

              /* File block (bits 0 - 30) and last block flag (bit 31) */
              proto_tree_add_item(point_tree, hf_dnp3_al_file_blocknum,  tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
              proto_tree_add_item(point_tree, hf_dnp3_al_file_lastblock, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
              data_pos += 4;

              /* File data */
              file_data_size = al_ptaddr - (data_pos - offset - prefixbytes);
              if ((file_data_size) > 0) {
                proto_tree_add_item(point_tree, hf_dnp3_al_file_data, tvb, data_pos, file_data_size, ENC_NA);
                data_pos += file_data_size;
              }

              proto_item_set_len(point_item, data_pos - offset);

              offset = data_pos;
              break;

            case AL_OBJ_FILE_TRAN_ST: /* File Control Tansport Status (Obj:70, Var:06) */

              /* File Handle */
              proto_tree_add_item(point_tree, hf_dnp3_al_file_handle, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
              data_pos += 4;

              /* File block (bits 0 - 30) and last block flag (bit 31) */
              proto_tree_add_item(point_tree, hf_dnp3_al_file_blocknum,  tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
              proto_tree_add_item(point_tree, hf_dnp3_al_file_lastblock, tvb, data_pos, 4, ENC_LITTLE_ENDIAN);
              data_pos += 4;

              /* Status code */
              proto_tree_add_item(point_tree, hf_dnp3_al_file_status, tvb, data_pos, 1, ENC_LITTLE_ENDIAN);
              data_pos += 1;

              /* Optional text */
              file_data_size = al_ptaddr - (data_pos - offset - prefixbytes);
              if ((file_data_size) > 0) {
                proto_tree_add_item(point_tree, hf_dnp3_al_file_data, tvb, data_pos, file_data_size, ENC_NA);
                data_pos += file_data_size;
              }

              proto_item_set_len(point_item, data_pos - offset);

              offset = data_pos;
              break;

            case AL_OBJ_OCT:      /* Octet string */
            case AL_OBJ_OCT_EVT:  /* Octet string event */

              /* read the number of bytes defined by the variation */
              if (al_oct_len > 0) {
                proto_tree_add_item(object_tree, hf_dnp3_al_octet_string, tvb, data_pos, al_oct_len, ENC_NA);
                data_pos += al_oct_len;
                proto_item_set_len(point_item, data_pos - offset);
              }

              offset = data_pos;
              break;

            default:             /* In case of unknown object */

              proto_tree_add_item(object_tree, hf_dnp3_unknown_data_chunk, tvb, offset, -1, ENC_NA);
              offset = tvb_captured_length(tvb); /* Finish decoding if unknown object is encountered... */
              break;
          }
        }

        /* And increment the point address, may be overwritten by an index value */
        al_ptaddr++;
      }
      else {
        /* No objects, just prefixes, move past prefix values */
        offset = data_pos;
      }
      if (start_offset > offset) {
        expert_add_info(pinfo, point_item, &ei_dnp_invalid_length);
        offset = tvb_captured_length(tvb); /* Finish decoding if unknown object is encountered... */
      }
    }
  }
  proto_item_set_len(object_item, offset - orig_offset);

  return offset;
}
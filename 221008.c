proto_register_dnp3(void)
{

/* Setup list of header fields */
  static hf_register_info hf[] = {
    { &hf_dnp3_start,
      { "Start Bytes", "dnp3.start",
        FT_UINT16, BASE_HEX, NULL, 0x0,
        NULL, HFILL }
    },

    { &hf_dnp3_len,
      { "Length", "dnp3.len",
        FT_UINT8, BASE_DEC, NULL, 0x0,
        "Frame Data Length", HFILL }
    },

    { &hf_dnp3_ctl,
      { "Control", "dnp3.ctl",
        FT_UINT8, BASE_HEX, NULL, 0x0,
        "Frame Control Byte", HFILL }
    },

    { &hf_dnp3_ctl_prifunc,
      { "Control Function Code", "dnp3.ctl.prifunc",
        FT_UINT8, BASE_DEC, VALS(dnp3_ctl_func_pri_vals), DNP3_CTL_FUNC,
        "Frame Control Function Code", HFILL }
    },

    { &hf_dnp3_ctl_secfunc,
      { "Control Function Code", "dnp3.ctl.secfunc",
        FT_UINT8, BASE_DEC, VALS(dnp3_ctl_func_sec_vals), DNP3_CTL_FUNC,
        "Frame Control Function Code", HFILL }
    },

    { &hf_dnp3_ctlobj_code_c,
      { "Operation Type", "dnp3.ctl.op",
        FT_UINT8, BASE_DEC, VALS(dnp3_al_ctlc_code_vals), AL_OBJCTLC_CODE,
        "Control Code, Operation Type", HFILL }
    },

    { &hf_dnp3_ctlobj_code_m,
      { "Queue / Clear Field", "dnp3.ctl.clr",
        FT_UINT8, BASE_DEC, VALS(dnp3_al_ctlc_misc_vals), AL_OBJCTLC_MISC,
        "Control Code, Clear Field", HFILL }
    },

    { &hf_dnp3_ctlobj_code_tc,
      { "Trip Control Code", "dnp3.ctl.trip",
        FT_UINT8, BASE_DEC, VALS(dnp3_al_ctlc_tc_vals), AL_OBJCTLC_TC,
        "Control Code, Trip Close Control", HFILL }
    },

    { &hf_dnp3_ctl_dir,
      { "Direction", "dnp3.ctl.dir",
        FT_BOOLEAN, 8, TFS(&tfs_set_notset), DNP3_CTL_DIR,
        NULL, HFILL }
    },

    { &hf_dnp3_ctl_prm,
      { "Primary", "dnp3.ctl.prm",
        FT_BOOLEAN, 8, TFS(&tfs_set_notset), DNP3_CTL_PRM,
        NULL, HFILL }
    },

    { &hf_dnp3_ctl_fcb,
      { "Frame Count Bit", "dnp3.ctl.fcb",
        FT_BOOLEAN, 8, TFS(&tfs_set_notset), DNP3_CTL_FCB,
        NULL, HFILL }
    },

    { &hf_dnp3_ctl_fcv,
      { "Frame Count Valid", "dnp3.ctl.fcv",
        FT_BOOLEAN, 8, TFS(&tfs_set_notset), DNP3_CTL_FCV,
        NULL, HFILL }
    },

    { &hf_dnp3_ctl_dfc,
      { "Data Flow Control", "dnp3.ctl.dfc",
        FT_BOOLEAN, 8, TFS(&tfs_set_notset), DNP3_CTL_DFC,
        NULL, HFILL }
    },

    { &hf_dnp3_dst,
      { "Destination", "dnp3.dst",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        "Destination Address", HFILL }
    },

    { &hf_dnp3_src,
      { "Source", "dnp3.src",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        "Source Address", HFILL }
    },

    { &hf_dnp3_addr,
      { "Address", "dnp3.addr",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        "Source or Destination Address", HFILL }
    },

    { &hf_dnp3_data_hdr_crc,
      { "Data Link Header checksum", "dnp3.hdr.CRC",
        FT_UINT16, BASE_HEX, NULL, 0x0,
        NULL, HFILL }
    },

    { &hf_dnp3_data_hdr_crc_status,
        { "Data Link Header Checksum Status", "dnp.hdr.CRC.status",
        FT_UINT8, BASE_NONE, VALS(proto_checksum_vals), 0x0,
        NULL, HFILL }
    },

    { &hf_dnp3_tr_ctl,
      { "Transport Control", "dnp3.tr.ctl",
        FT_UINT8, BASE_HEX, NULL, 0x0,
        "Transport Layer Control Byte", HFILL }
    },

    { &hf_dnp3_tr_fin,
      { "Final", "dnp3.tr.fin",
        FT_BOOLEAN, 8, TFS(&tfs_set_notset), DNP3_TR_FIN,
        NULL, HFILL }
    },

    { &hf_dnp3_tr_fir,
      { "First", "dnp3.tr.fir",
        FT_BOOLEAN, 8, TFS(&tfs_set_notset), DNP3_TR_FIR,
        NULL, HFILL }
    },

    { &hf_dnp3_tr_seq,
      { "Sequence", "dnp3.tr.seq",
        FT_UINT8, BASE_DEC, NULL, DNP3_TR_SEQ,
        "Frame Sequence Number", HFILL }
    },

    { &hf_dnp3_data_chunk,
      { "Data Chunk", "dnp.data_chunk",
        FT_BYTES, BASE_NONE, NULL, 0x0,
        NULL, HFILL }
    },

    { &hf_dnp3_data_chunk_len,
      { "Data Chunk length", "dnp.data_chunk_len",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        NULL, HFILL }
    },

    { &hf_dnp3_data_chunk_crc,
      { "Data Chunk checksum", "dnp.data_chunk.CRC",
        FT_UINT16, BASE_HEX, NULL, 0x0,
        NULL, HFILL }
    },

    { &hf_dnp3_data_chunk_crc_status,
        { "Data Chunk Checksum Status", "dnp.data_chunk.CRC.status",
        FT_UINT8, BASE_NONE, VALS(proto_checksum_vals), 0x0,
        NULL, HFILL }
    },

    { &hf_dnp3_al_ctl,
      { "Application Control", "dnp3.al.ctl",
        FT_UINT8, BASE_HEX, NULL, 0x0,
        "Application Layer Control Byte", HFILL }
    },

    { &hf_dnp3_al_fir,
      { "First", "dnp3.al.fir",
        FT_BOOLEAN, 8, TFS(&tfs_set_notset), DNP3_AL_FIR,
        NULL, HFILL }
    },

    { &hf_dnp3_al_fin,
      { "Final", "dnp3.al.fin",
        FT_BOOLEAN, 8, TFS(&tfs_set_notset), DNP3_AL_FIN,
        NULL, HFILL }
    },

    { &hf_dnp3_al_con,
      { "Confirm", "dnp3.al.con",
        FT_BOOLEAN, 8, TFS(&tfs_set_notset), DNP3_AL_CON,
        NULL, HFILL }
    },

    { &hf_dnp3_al_uns,
      { "Unsolicited", "dnp3.al.uns",
        FT_BOOLEAN, 8, TFS(&tfs_set_notset), DNP3_AL_UNS,
        NULL, HFILL }
    },

    { &hf_dnp3_al_seq,
      { "Sequence", "dnp3.al.seq",
        FT_UINT8, BASE_DEC, NULL, DNP3_AL_SEQ,
        "Frame Sequence Number", HFILL }
    },

    { &hf_dnp3_al_func,
      { "Application Layer Function Code", "dnp3.al.func",
        FT_UINT8, BASE_DEC|BASE_EXT_STRING, &dnp3_al_func_vals_ext, DNP3_AL_FUNC,
        "Application Function Code", HFILL }
    },

    { &hf_dnp3_al_iin,
      { "Internal Indications", "dnp3.al.iin",
        FT_UINT16, BASE_HEX, NULL, 0x0,
        "Application Layer IIN", HFILL }
    },

    { &hf_dnp3_al_iin_bmsg,
      { "Broadcast Msg Rx", "dnp3.al.iin.bmsg",
        FT_BOOLEAN, 16, TFS(&tfs_set_notset), AL_IIN_BMSG,
        NULL, HFILL }
    },

    { &hf_dnp3_al_iin_cls1d,
      { "Class 1 Data Available", "dnp3.al.iin.cls1d",
        FT_BOOLEAN, 16, TFS(&tfs_set_notset), AL_IIN_CLS1D,
        NULL, HFILL }
    },

    { &hf_dnp3_al_iin_cls2d,
      { "Class 2 Data Available", "dnp3.al.iin.cls2d",
        FT_BOOLEAN, 16, TFS(&tfs_set_notset), AL_IIN_CLS2D,
        NULL, HFILL }
    },

    { &hf_dnp3_al_iin_cls3d,
      { "Class 3 Data Available", "dnp3.al.iin.cls3d",
        FT_BOOLEAN, 16, TFS(&tfs_set_notset), AL_IIN_CLS3D,
        NULL, HFILL }
    },

    { &hf_dnp3_al_iin_tsr,
      { "Time Sync Required", "dnp3.al.iin.tsr",
        FT_BOOLEAN, 16, TFS(&tfs_set_notset), AL_IIN_TSR,
        NULL, HFILL }
    },

    { &hf_dnp3_al_iin_dol,
      { "Digital Outputs in Local", "dnp3.al.iin.dol",
        FT_BOOLEAN, 16, TFS(&tfs_set_notset), AL_IIN_DOL,
        NULL, HFILL }
    },

    { &hf_dnp3_al_iin_dt,
      { "Device Trouble", "dnp3.al.iin.dt",
        FT_BOOLEAN, 16, TFS(&tfs_set_notset), AL_IIN_DT,
        NULL, HFILL }
    },

    { &hf_dnp3_al_iin_rst,
      { "Device Restart", "dnp3.al.iin.rst",
        FT_BOOLEAN, 16, TFS(&tfs_set_notset), AL_IIN_RST,
        NULL, HFILL }
    },

    { &hf_dnp3_al_iin_fcni,
      { "Function Code not implemented", "dnp3.al.iin.fcni",
        FT_BOOLEAN, 16, TFS(&tfs_set_notset), AL_IIN_FCNI,
        NULL, HFILL }
    },

    { &hf_dnp3_al_iin_obju,
      { "Requested Objects Unknown", "dnp3.al.iin.obju",
        FT_BOOLEAN, 16, TFS(&tfs_set_notset), AL_IIN_OBJU,
        NULL, HFILL }
    },

    { &hf_dnp3_al_iin_pioor,
      { "Parameters Invalid or Out of Range", "dnp3.al.iin.pioor",
        FT_BOOLEAN, 16, TFS(&tfs_set_notset), AL_IIN_PIOOR,
        NULL, HFILL }
    },

    { &hf_dnp3_al_iin_ebo,
      { "Event Buffer Overflow", "dnp3.al.iin.ebo",
        FT_BOOLEAN, 16, TFS(&tfs_set_notset), AL_IIN_EBO,
        NULL, HFILL }
    },

    { &hf_dnp3_al_iin_oae,
      { "Operation Already Executing", "dnp3.al.iin.oae",
        FT_BOOLEAN, 16, TFS(&tfs_set_notset), AL_IIN_OAE,
        NULL, HFILL }
    },

    { &hf_dnp3_al_iin_cc,
      { "Configuration Corrupt", "dnp3.al.iin.cc",
        FT_BOOLEAN, 16, TFS(&tfs_set_notset), AL_IIN_CC,
        NULL, HFILL }
    },

    { &hf_dnp3_al_obj,
      { "Object", "dnp3.al.obj",
        FT_UINT16, BASE_HEX|BASE_EXT_STRING, &dnp3_al_obj_vals_ext, 0x0,
        "Application Layer Object", HFILL }
    },

    { &hf_dnp3_al_objq_prefix,
      { "Prefix Code", "dnp3.al.objq.prefix",
        FT_UINT8, BASE_DEC|BASE_EXT_STRING, &dnp3_al_objq_prefix_vals_ext, AL_OBJQ_PREFIX,
        "Object Prefix Code", HFILL }
    },

    { &hf_dnp3_al_objq_range,
      { "Range Code", "dnp3.al.objq.range",
        FT_UINT8, BASE_DEC|BASE_EXT_STRING, &dnp3_al_objq_range_vals_ext, AL_OBJQ_RANGE,
        "Object Range Specifier Code", HFILL }
    },

    { &hf_dnp3_al_range_start8,
      { "Start (8 bit)", "dnp3.al.range.start",
        FT_UINT8, BASE_DEC, NULL, 0x0,
        "Object Start Index", HFILL }
    },

    { &hf_dnp3_al_range_stop8,
      { "Stop (8 bit)", "dnp3.al.range.stop",
        FT_UINT8, BASE_DEC, NULL, 0x0,
        "Object Stop Index", HFILL }
    },

    { &hf_dnp3_al_range_start16,
      { "Start (16 bit)", "dnp3.al.range.start",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        "Object Start Index", HFILL }
    },

    { &hf_dnp3_al_range_stop16,
      { "Stop (16 bit)", "dnp3.al.range.stop",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        "Object Stop Index", HFILL }
    },

    { &hf_dnp3_al_range_start32,
      { "Start (32 bit)", "dnp3.al.range.start",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Object Start Index", HFILL }
    },

    { &hf_dnp3_al_range_stop32,
      { "Stop (32 bit)", "dnp3.al.range.stop",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Object Stop Index", HFILL }
    },

    { &hf_dnp3_al_range_abs8,
      { "Address (8 bit)", "dnp3.al.range.abs",
        FT_UINT8, BASE_DEC, NULL, 0x0,
        "Object Absolute Address", HFILL }
    },

    { &hf_dnp3_al_range_abs16,
      { "Address (16 bit)", "dnp3.al.range.abs",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        "Object Absolute Address", HFILL }
    },

    { &hf_dnp3_al_range_abs32,
      { "Address (32 bit)", "dnp3.al.range.abs",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Object Absolute Address", HFILL }
    },

    { &hf_dnp3_al_range_quant8,
      { "Quantity (8 bit)", "dnp3.al.range.quantity",
        FT_UINT8, BASE_DEC, NULL, 0x0,
        "Object Quantity", HFILL }
    },

    { &hf_dnp3_al_range_quant16,
      { "Quantity (16 bit)", "dnp3.al.range.quantity",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        "Object Quantity", HFILL }
    },

    { &hf_dnp3_al_range_quant32,
      { "Quantity (32 bit)", "dnp3.al.range.quantity",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Object Quantity", HFILL }
    },

    { &hf_dnp3_al_index8,
      { "Index (8 bit)", "dnp3.al.index",
        FT_UINT8, BASE_DEC, NULL, 0x0,
        "Object Index", HFILL }
    },

    { &hf_dnp3_al_index16,
      { "Index (16 bit)", "dnp3.al.index",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        "Object Index", HFILL }
    },

    { &hf_dnp3_al_index32,
      { "Index (32 bit)", "dnp3.al.index",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Object Index", HFILL }
    },

#if 0
    { &hf_dnp3_al_ptnum,
      { "Object Point Number", "dnp3.al.ptnum",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        NULL, HFILL }
    },
#endif

    { &hf_dnp3_al_size8,
      { "Size (8 bit)", "dnp3.al.size",
          FT_UINT8, BASE_DEC, NULL, 0x0,
          "Object Size", HFILL }
    },

    { &hf_dnp3_al_size16,
      { "Size (16 bit)", "dnp3.al.size",
          FT_UINT16, BASE_DEC, NULL, 0x0,
          "Object Size", HFILL }
    },

    { &hf_dnp3_al_size32,
      { "Size (32 bit)", "dnp3.al.size",
          FT_UINT32, BASE_DEC, NULL, 0x0,
          "Object Size", HFILL }
    },

    { &hf_dnp3_bocs_bit,
      { "Commanded State", "dnp3.al.bocs",
          FT_BOOLEAN, 8, TFS(&tfs_on_off), 0x80,
          "Binary Output Commanded state", HFILL }
    },

    { &hf_dnp3_al_bit,
      { "Value (bit)", "dnp3.al.bit",
          FT_BOOLEAN, 8, TFS(&tfs_on_off), 0x1,
          "Digital Value (1 bit)", HFILL }
    },

    { &hf_dnp3_al_2bit,
      { "Value (two bit)", "dnp3.al.2bit",
          FT_UINT8, BASE_DEC, NULL, 0x0,
          "Digital Value (2 bit)", HFILL }
    },

    { &hf_dnp3_al_ana16,
      { "Value (16 bit)", "dnp3.al.ana.int",
          FT_INT16, BASE_DEC, NULL, 0x0,
          "Analog Value (16 bit)", HFILL }
    },

    { &hf_dnp3_al_ana32,
      { "Value (32 bit)", "dnp3.al.ana.int",
          FT_INT32, BASE_DEC, NULL, 0x0,
          "Analog Value (32 bit)", HFILL }
    },

    { &hf_dnp3_al_anaflt,
      { "Value (float)", "dnp3.al.ana.float",
          FT_FLOAT, BASE_NONE, NULL, 0x0,
          "Analog Value (float)", HFILL }
    },

    { &hf_dnp3_al_anadbl,
      { "Value (double)", "dnp3.al.ana.double",
          FT_DOUBLE, BASE_NONE, NULL, 0x0,
          "Analog Value (double)", HFILL }
    },

    { &hf_dnp3_al_anaout16,
      { "Output Value (16 bit)", "dnp3.al.anaout.int",
          FT_INT16, BASE_DEC, NULL, 0x0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_anaout32,
      { "Output Value (32 bit)", "dnp3.al.anaout.int",
          FT_INT32, BASE_DEC, NULL, 0x0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_anaoutflt,
      { "Output Value (float)", "dnp3.al.anaout.float",
          FT_FLOAT, BASE_NONE, NULL, 0x0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_anaoutdbl,
      { "Output (double)", "dnp3.al.anaout.double",
          FT_DOUBLE, BASE_NONE, NULL, 0x0,
          "Output Value (double)", HFILL }
    },

    { &hf_dnp3_al_cnt16,
      { "Counter (16 bit)", "dnp3.al.cnt",
          FT_UINT16, BASE_DEC, NULL, 0x0,
          "Counter Value (16 bit)", HFILL }
    },

    { &hf_dnp3_al_cnt32,
      { "Counter (32 bit)", "dnp3.al.cnt",
          FT_UINT32, BASE_DEC, NULL, 0x0,
          "Counter Value (32 bit)", HFILL }
    },

    { &hf_dnp3_al_ctrlstatus,
      { "Control Status", "dnp3.al.ctrlstatus",
          FT_UINT8, BASE_DEC|BASE_EXT_STRING, &dnp3_al_ctl_status_vals_ext, AL_OBJCTL_STATUS_MASK,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_mode,
      { "File Control Mode", "dnp3.al.file.mode",
          FT_UINT16, BASE_DEC, VALS(dnp3_al_file_mode_vals), 0x0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_auth,
      { "File Authentication Key", "dnp3.al.file.auth",
          FT_UINT32, BASE_HEX, NULL, 0x0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_size,
      { "File Size", "dnp3.al.file.size",
          FT_UINT32, BASE_HEX, NULL, 0x0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_maxblk,
      { "File Max Block Size", "dnp3.al.file.maxblock",
          FT_UINT16, BASE_DEC, NULL, 0x0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_reqID,
      { "File Request Identifier", "dnp3.al.file.reqID",
          FT_UINT16, BASE_DEC, NULL, 0x0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_status,
      { "File Control Status", "dnp3.al.file.status",
          FT_UINT8, BASE_DEC|BASE_EXT_STRING, &dnp3_al_file_status_vals_ext, 0x0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_handle,
      { "File Handle", "dnp3.al.file.handle",
          FT_UINT32, BASE_HEX, NULL, 0x0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_blocknum,
      { "File Block Number", "dnp3.al.file.blocknum",
          FT_UINT32, BASE_HEX, NULL, 0x7fffffff,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_lastblock,
      { "File Last Block", "dnp3.al.file.lastblock",
          FT_BOOLEAN, 32, TFS(&tfs_set_notset), 0x80000000,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_data,
      { "File Data", "dnp3.al.file.data",
          FT_BYTES, BASE_NONE, NULL, 0x0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_biq_b0,
      { "Online", "dnp3.al.biq.b0",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_BI_FLAG0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_biq_b1,
      { "Restart", "dnp3.al.biq.b1",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_BI_FLAG1,
          NULL, HFILL }
    },

    { &hf_dnp3_al_biq_b2,
      { "Comm Fail", "dnp3.al.biq.b2",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_BI_FLAG2,
          NULL, HFILL }
    },

    { &hf_dnp3_al_biq_b3,
      { "Remote Force", "dnp3.al.biq.b3",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_BI_FLAG3,
          NULL, HFILL }
    },

    { &hf_dnp3_al_biq_b4,
      { "Local Force", "dnp3.al.biq.b4",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_BI_FLAG4,
          NULL, HFILL }
    },

    { &hf_dnp3_al_biq_b5,
      { "Chatter Filter", "dnp3.al.biq.b5",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_BI_FLAG5,
          NULL, HFILL }
    },

    { &hf_dnp3_al_biq_b6,
      { "Reserved", "dnp3.al.biq.b6",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_BI_FLAG6,
          NULL, HFILL }
    },

    { &hf_dnp3_al_biq_b7,
      { "Point Value", "dnp3.al.biq.b7",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_BI_FLAG7,
          NULL, HFILL }
    },

    { &hf_dnp3_al_boq_b0,
      { "Online", "dnp3.al.boq.b0",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_BO_FLAG0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_boq_b1,
      { "Restart", "dnp3.al.boq.b1",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_BO_FLAG1,
          NULL, HFILL }
    },

    { &hf_dnp3_al_boq_b2,
      { "Comm Fail", "dnp3.al.boq.b2",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_BO_FLAG2,
          NULL, HFILL }
    },

    { &hf_dnp3_al_boq_b3,
      { "Remote Force", "dnp3.al.boq.b3",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_BO_FLAG3,
          NULL, HFILL }
    },

    { &hf_dnp3_al_boq_b4,
      { "Local Force", "dnp3.al.boq.b4",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_BO_FLAG4,
          NULL, HFILL }
    },

    { &hf_dnp3_al_boq_b5,
      { "Reserved", "dnp3.al.boq.b5",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_BO_FLAG5,
          NULL, HFILL }
    },

    { &hf_dnp3_al_boq_b6,
      { "Reserved", "dnp3.al.boq.b6",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_BO_FLAG6,
          NULL, HFILL }
    },

    { &hf_dnp3_al_boq_b7,
      { "Point Value", "dnp3.al.boq.b7",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_BO_FLAG7,
          NULL, HFILL }
    },

    { &hf_dnp3_al_ctrq_b0,
      { "Online", "dnp3.al.ctrq.b0",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_CTR_FLAG0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_ctrq_b1,
      { "Restart", "dnp3.al.ctrq.b1",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_CTR_FLAG1,
          NULL, HFILL }
    },

    { &hf_dnp3_al_ctrq_b2,
      { "Comm Fail", "dnp3.al.ctrq.b2",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_CTR_FLAG2,
          NULL, HFILL }
    },

    { &hf_dnp3_al_ctrq_b3,
      { "Remote Force", "dnp3.al.ctrq.b3",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_CTR_FLAG3,
          NULL, HFILL }
    },

    { &hf_dnp3_al_ctrq_b4,
      { "Local Force", "dnp3.al.ctrq.b4",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_CTR_FLAG4,
          NULL, HFILL }
    },

    { &hf_dnp3_al_ctrq_b5,
      { "Roll-Over", "dnp3.al.ctrq.b5",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_CTR_FLAG5,
          NULL, HFILL }
    },

    { &hf_dnp3_al_ctrq_b6,
      { "Discontinuity", "dnp3.al.ctrq.b6",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_CTR_FLAG6,
          NULL, HFILL }
    },

    { &hf_dnp3_al_ctrq_b7,
      { "Reserved", "dnp3.al.ctrq.b7",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_CTR_FLAG7,
          NULL, HFILL }
    },

    { &hf_dnp3_al_aiq_b0,
      { "Online", "dnp3.al.aiq.b0",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_AI_FLAG0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_aiq_b1,
      { "Restart", "dnp3.al.aiq.b1",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_AI_FLAG1,
          NULL, HFILL }
    },

    { &hf_dnp3_al_aiq_b2,
      { "Comm Fail", "dnp3.al.aiq.b2",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_AI_FLAG2,
          NULL, HFILL }
    },

    { &hf_dnp3_al_aiq_b3,
      { "Remote Force", "dnp3.al.aiq.b3",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_AI_FLAG3,
          NULL, HFILL }
    },

    { &hf_dnp3_al_aiq_b4,
      { "Local Force", "dnp3.al.aiq.b4",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_AI_FLAG4,
          NULL, HFILL }
    },

    { &hf_dnp3_al_aiq_b5,
      { "Over-Range", "dnp3.al.aiq.b5",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_AI_FLAG5,
          NULL, HFILL }
    },

    { &hf_dnp3_al_aiq_b6,
      { "Reference Check", "dnp3.al.aiq.b6",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_AI_FLAG6,
          NULL, HFILL }
    },

    { &hf_dnp3_al_aiq_b7,
      { "Reserved", "dnp3.al.aiq.b7",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_AI_FLAG7,
          NULL, HFILL }
    },

    { &hf_dnp3_al_aoq_b0,
      { "Online", "dnp3.al.aoq.b0",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_AO_FLAG0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_aoq_b1,
      { "Restart", "dnp3.al.aoq.b1",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_AO_FLAG1,
          NULL, HFILL }
    },

    { &hf_dnp3_al_aoq_b2,
      { "Comm Fail", "dnp3.al.aoq.b2",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_AO_FLAG2,
          NULL, HFILL }
    },

    { &hf_dnp3_al_aoq_b3,
      { "Remote Force", "dnp3.al.aoq.b3",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_AO_FLAG3,
          NULL, HFILL }
    },

    { &hf_dnp3_al_aoq_b4,
      { "Local Force", "dnp3.al.aoq.b4",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_AO_FLAG4,
          NULL, HFILL }
    },

    { &hf_dnp3_al_aoq_b5,
      { "Reserved", "dnp3.al.aoq.b5",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_AO_FLAG5,
          NULL, HFILL }
    },

    { &hf_dnp3_al_aoq_b6,
      { "Reserved", "dnp3.al.aoq.b6",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_AO_FLAG6,
          NULL, HFILL }
    },

    { &hf_dnp3_al_aoq_b7,
      { "Reserved", "dnp3.al.aoq.b7",
          FT_BOOLEAN, 8, TFS(&tfs_set_notset), AL_OBJ_AO_FLAG7,
          NULL, HFILL }
    },

    { &hf_dnp3_al_timestamp,
      { "Timestamp", "dnp3.al.timestamp",
          FT_ABSOLUTE_TIME, ABSOLUTE_TIME_UTC, NULL, 0,
          "Object Timestamp", HFILL }
    },

    { &hf_dnp3_al_file_perms,
      { "Permissions", "dnp3.al.file.perms",
          FT_UINT16, BASE_OCT, NULL, 0x0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_perms_read_owner,
      { "Read permission for owner", "dnp3.al.file.perms.read_owner",
          FT_BOOLEAN, 16, TFS(&tfs_yes_no), 0400,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_perms_write_owner,
      { "Write permission for owner", "dnp3.al.file.perms.write_owner",
          FT_BOOLEAN, 16, TFS(&tfs_yes_no), 0200,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_perms_exec_owner,
      { "Execute permission for owner", "dnp3.al.file.perms.exec_owner",
          FT_BOOLEAN, 16, TFS(&tfs_yes_no), 0100,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_perms_read_group,
      { "Read permission for group", "dnp3.al.file.perms.read_group",
          FT_BOOLEAN, 16, TFS(&tfs_yes_no), 040,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_perms_write_group,
      { "Write permission for group", "dnp3.al.file.perms.write_group",
          FT_BOOLEAN, 16, TFS(&tfs_yes_no), 020,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_perms_exec_group,
      { "Execute permission for group", "dnp3.al.file.perms.exec_group",
          FT_BOOLEAN, 16, TFS(&tfs_yes_no), 010,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_perms_read_world,
      { "Read permission for world", "dnp3.al.file.perms.read_world",
          FT_BOOLEAN, 16, TFS(&tfs_yes_no), 04,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_perms_write_world,
      { "Write permission for world", "dnp3.al.file.perms.write_world",
          FT_BOOLEAN, 16, TFS(&tfs_yes_no), 02,
          NULL, HFILL }
    },

    { &hf_dnp3_al_file_perms_exec_world,
      { "Execute permission for world", "dnp3.al.file.perms.exec_world",
          FT_BOOLEAN, 16, TFS(&tfs_yes_no), 01,
          NULL, HFILL }
    },

    { &hf_dnp3_al_rel_timestamp,
      { "Relative Timestamp", "dnp3.al.reltimestamp",
          FT_RELATIVE_TIME, BASE_NONE, NULL, 0,
          "Object Relative Timestamp", HFILL }
    },

    { &hf_dnp3_al_datatype,
      { "Data Type", "dnp3.al.datatype",
          FT_UINT8, BASE_HEX, VALS(dnp3_al_data_type_vals), 0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_da_length,
      { "Device Attribute Length", "dnp3.al.da.length",
          FT_UINT8, BASE_DEC, NULL, 0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_da_uint8,
      { "Device Attribute 8-Bit Unsigned Integer Value", "dnp3.al.da.uint8",
          FT_UINT8, BASE_DEC, NULL, 0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_da_uint16,
      { "Device Attribute 16-Bit Unsigned Integer Value", "dnp3.al.da.uint16",
          FT_UINT16, BASE_DEC, NULL, 0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_da_uint32,
      { "Device Attribute 32-Bit Unsigned Integer Value", "dnp3.al.da.uint32",
          FT_UINT32, BASE_DEC, NULL, 0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_da_int8,
      { "Device Attribute 8-Bit Integer Value", "dnp3.al.da.int8",
          FT_INT8, BASE_DEC, NULL, 0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_da_int16,
      { "Device Attribute 16-Bit Integer Value", "dnp3.al.da.int16",
          FT_INT16, BASE_DEC, NULL, 0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_da_int32,
      { "Device Attribute 32-Bit Integer Value", "dnp3.al.da.int32",
          FT_INT32, BASE_DEC, NULL, 0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_da_flt,
      { "Device Attribute Float Value", "dnp3.al.da.float",
          FT_FLOAT, BASE_NONE, NULL, 0,
          NULL, HFILL }
    },

    { &hf_dnp3_al_da_dbl,
      { "Device Attribute Double Value", "dnp3.al.da.double",
          FT_DOUBLE, BASE_NONE, NULL, 0,
          NULL, HFILL }
    },

    { &hf_al_frag_data,
      {"DNP3.0 AL Fragment Data", "dnp3.al.frag_data",
          FT_BYTES, BASE_NONE, NULL, 0x00,
          "DNP 3.0 Application Layer Fragment Data", HFILL }},

    { &hf_dnp3_fragment,
      { "DNP 3.0 AL Fragment", "dnp3.al.fragment",
          FT_FRAMENUM, BASE_NONE, NULL, 0x0,
          "DNP 3.0 Application Layer Fragment", HFILL }
    },

    { &hf_dnp3_fragments,
      { "DNP 3.0 AL Fragments", "dnp3.al.fragments",
          FT_NONE, BASE_NONE, NULL, 0x0,
          "DNP 3.0 Application Layer Fragments", HFILL }
    },

    { &hf_dnp3_fragment_overlap,
      { "Fragment overlap", "dnp3.al.fragment.overlap",
          FT_BOOLEAN, BASE_NONE, NULL, 0x0,
          "Fragment overlaps with other fragments", HFILL }
    },

    { &hf_dnp3_fragment_overlap_conflict,
      { "Conflicting data in fragment overlap", "dnp3.al.fragment.overlap.conflict",
          FT_BOOLEAN, BASE_NONE, NULL, 0x0,
          "Overlapping fragments contained conflicting data", HFILL }
    },

    { &hf_dnp3_fragment_multiple_tails,
      { "Multiple tail fragments found", "dnp3.al.fragment.multipletails",
          FT_BOOLEAN, BASE_NONE, NULL, 0x0,
          "Several tails were found when defragmenting the packet", HFILL }
    },

    { &hf_dnp3_fragment_too_long_fragment,
      { "Fragment too long", "dnp3.al.fragment.toolongfragment",
          FT_BOOLEAN, BASE_NONE, NULL, 0x0,
          "Fragment contained data past end of packet", HFILL }
    },

    { &hf_dnp3_fragment_error,
      { "Defragmentation error", "dnp3.al.fragment.error",
        FT_FRAMENUM, BASE_NONE, NULL, 0x0,
        "Defragmentation error due to illegal fragments", HFILL }
    },

    { &hf_dnp3_fragment_count,
      { "Fragment count", "dnp3.al.fragment.count",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL }
    },

    { &hf_dnp3_fragment_reassembled_in,
      { "Reassembled PDU In Frame", "dnp3.al.fragment.reassembled_in",
        FT_FRAMENUM, BASE_NONE, NULL, 0x0,
        "This PDU is reassembled in this frame", HFILL }
    },

    { &hf_dnp3_fragment_reassembled_length,
      { "Reassembled DNP length", "dnp3.al.fragment.reassembled.length",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The total length of the reassembled payload", HFILL }
    },
    /* Generated from convert_proto_tree_add_text.pl */
    { &hf_dnp3_al_point_index, { "Point Index", "dnp3.al.point_index", FT_UINT32, BASE_DEC, NULL, 0x0, NULL, HFILL }},
    { &hf_dnp3_al_da_value, { "Value", "dnp3.al.da.value", FT_STRING, BASE_NONE, NULL, 0x0, NULL, HFILL }},
    { &hf_dnp3_al_count, { "Count", "dnp3.al.count", FT_UINT8, BASE_DEC, NULL, 0x0, NULL, HFILL }},
    { &hf_dnp3_al_on_time, { "On Time", "dnp3.al.on_time", FT_UINT32, BASE_DEC, NULL, 0x0, NULL, HFILL }},
    { &hf_dnp3_al_off_time, { "Off Time", "dnp3.al.off_time", FT_UINT32, BASE_DEC, NULL, 0x0, NULL, HFILL }},
    { &hf_dnp3_al_time_delay, { "Time Delay", "dnp3.al.time_delay", FT_UINT16, BASE_DEC|BASE_UNIT_STRING, &units_milliseconds, 0x0, NULL, HFILL }},
    { &hf_dnp3_al_file_string_offset, { "File String Offset", "dnp3.al.file_string_offset", FT_UINT16, BASE_DEC, NULL, 0x0, NULL, HFILL }},
    { &hf_dnp3_al_file_string_length, { "File String Length", "dnp3.al.file_string_length", FT_UINT16, BASE_DEC, NULL, 0x0, NULL, HFILL }},
    { &hf_dnp3_al_file_name, { "File Name", "dnp3.al.file_name", FT_STRING, BASE_NONE, NULL, 0x0, NULL, HFILL }},
    { &hf_dnp3_al_octet_string, { "Octet String", "dnp3.al.octet_string", FT_BYTES, BASE_NONE, NULL, 0x0, NULL, HFILL }},
    { &hf_dnp3_unknown_data_chunk, { "Unknown Data Chunk", "dnp3.al.unknown_data_chunk", FT_BYTES, BASE_NONE, NULL, 0x0, NULL, HFILL }},

  };

/* Setup protocol subtree array */
  static gint *ett[] = {
    &ett_dnp3,
    &ett_dnp3_dl,
    &ett_dnp3_dl_ctl,
    &ett_dnp3_tr_ctl,
    &ett_dnp3_dl_data,
    &ett_dnp3_dl_chunk,
    &ett_dnp3_al,
    &ett_dnp3_al_ctl,
    &ett_dnp3_al_obj_point_tcc,
    &ett_dnp3_al_iin,
    &ett_dnp3_al_obj,
    &ett_dnp3_al_obj_qualifier,
    &ett_dnp3_al_obj_range,
    &ett_dnp3_al_objdet,
    &ett_dnp3_al_obj_quality,
    &ett_dnp3_al_obj_point,
    &ett_dnp3_al_obj_point_perms,
    &ett_dnp3_fragment,
    &ett_dnp3_fragments
  };
  static ei_register_info ei[] = {
     { &ei_dnp_num_items_neg, { "dnp3.num_items_neg", PI_MALFORMED, PI_ERROR, "Negative number of items", EXPFILL }},
     { &ei_dnp_invalid_length, { "dnp3.invalid_length", PI_MALFORMED, PI_ERROR, "Invalid length", EXPFILL }},
     { &ei_dnp_iin_abnormal, { "dnp3.iin_abnormal", PI_PROTOCOL, PI_WARN, "IIN Abnormality", EXPFILL }},
     { &ei_dnp3_data_hdr_crc_incorrect, { "dnp3.hdr.CRC.incorrect", PI_CHECKSUM, PI_WARN, "Data Link Header Checksum incorrect", EXPFILL }},
     { &ei_dnp3_data_chunk_crc_incorrect, { "dnp3.data_chunk.CRC.incorrect", PI_CHECKSUM, PI_WARN, "Data Chunk Checksum incorrect", EXPFILL }},
     { &ei_dnp3_unknown_object, { "dnp3.unknown_object", PI_PROTOCOL, PI_WARN, "Unknown Object\\Variation", EXPFILL }},
     { &ei_dnp3_unknown_group0_variation, { "dnp3.unknown_group0_variation", PI_PROTOCOL, PI_WARN, "Unknown Group 0 Variation", EXPFILL }},
     { &ei_dnp3_num_items_invalid, { "dnp3.num_items_invalid", PI_MALFORMED, PI_ERROR, "Number of items is invalid for normally empty object.  Potentially malicious packet", EXPFILL }},
      /* Generated from convert_proto_tree_add_text.pl */
#if 0
      { &ei_dnp3_buffering_user_data_until_final_frame_is_received, { "dnp3.buffering_user_data_until_final_frame_is_received", PI_PROTOCOL, PI_WARN, "Buffering User Data Until Final Frame is Received..", EXPFILL }},
#endif
    };

  module_t *dnp3_module;
  expert_module_t* expert_dnp3;

  reassembly_table_register(&al_reassembly_table,
                        &addresses_reassembly_table_functions);

/* Register the protocol name and description */
  proto_dnp3 = proto_register_protocol("Distributed Network Protocol 3.0", "DNP 3.0", "dnp3");

/* Register the dissector so it may be used as a User DLT payload protocol */
  dnp3_tcp_handle = register_dissector("dnp3.tcp", dissect_dnp3_tcp, proto_dnp3);
  dnp3_udp_handle = register_dissector("dnp3.udp", dissect_dnp3_udp, proto_dnp3);

/* Required function calls to register the header fields and subtrees used */
  proto_register_field_array(proto_dnp3, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));
  expert_dnp3 = expert_register_protocol(proto_dnp3);
  expert_register_field_array(expert_dnp3, ei, array_length(ei));

  dnp3_module = prefs_register_protocol(proto_dnp3, NULL);
  prefs_register_obsolete_preference(dnp3_module, "heuristics");
  prefs_register_bool_preference(dnp3_module, "desegment",
    "Reassemble DNP3 messages spanning multiple TCP segments",
    "Whether the DNP3 dissector should reassemble messages spanning multiple TCP segments."
    " To use this option, you must also enable \"Allow subdissectors to reassemble TCP streams\" in the TCP protocol settings.",
    &dnp3_desegment);
}
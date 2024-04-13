void APar_Extract_esds_Info(char *uint32_buffer,
                            FILE *isofile,
                            short track_level_atom,
                            TrackInfo *track_info) {
  uint64_t offset_into_stsd = 0;

  while (offset_into_stsd < parsedAtoms[track_level_atom].AtomicLength) {
    offset_into_stsd++;
    if (APar_read32(uint32_buffer,
                    isofile,
                    parsedAtoms[track_level_atom].AtomicStart +
                        offset_into_stsd) == 0x65736473) {
      track_info->contains_esds = true;

      uint64_t esds_start =
          parsedAtoms[track_level_atom].AtomicStart + offset_into_stsd - 4;
      uint64_t esds_length = APar_read32(uint32_buffer, isofile, esds_start);
      uint64_t offset_into_esds =
          12; // 4bytes length + 4 bytes name + 4bytes null

      if (APar_read8(isofile, esds_start + offset_into_esds) == 0x03) {
        offset_into_esds++;
        offset_into_esds +=
            APar_skip_filler(isofile, esds_start + offset_into_esds);
      }

      uint8_t section3_length =
          APar_read8(isofile, esds_start + offset_into_esds);
      if (section3_length <= esds_length && section3_length != 0) {
        track_info->section3_length = section3_length;
      } else {
        break;
      }

      // for whatever reason, when mp4box muxes in ogg into an mp4 container,
      // section 3 gets a 0x9D byte (which doesn't fall inline with what AP
      // considers 'filler') then again, I haven't *completely* read the ISO
      // specifications, so I could just be missing it the the ->voluminous<-
      // 14496-X specifications.
      uint8_t test_byte =
          APar_read8(isofile, esds_start + offset_into_esds + 1);
      if (test_byte != 0) {
        offset_into_esds++;
      }

      offset_into_esds += 4; // 1 bytes section 0x03 length + 2 bytes + 1 byte

      if (APar_read8(isofile, esds_start + offset_into_esds) == 0x04) {
        offset_into_esds++;
        offset_into_esds +=
            APar_skip_filler(isofile, esds_start + offset_into_esds);
      }

      uint8_t section4_length =
          APar_read8(isofile, esds_start + offset_into_esds);
      if (section4_length <= section3_length && section4_length != 0) {
        track_info->section4_length = section4_length;

        if (section4_length == 0x9D)
          offset_into_esds++; // upper limit? when gpac puts an ogg in, section
                              // 3 is 9D - so is sec4 (section 4 real length
                              // with ogg = 0x0E86)

        offset_into_esds++;
        track_info->ObjectTypeIndication =
            APar_read8(isofile, esds_start + offset_into_esds);

        // this is just so that ogg in mp4 won't have some bizarre high bitrate
        // of like 2.8megabits/sec
        uint8_t a_v_flag =
            APar_read8(isofile,
                       esds_start + offset_into_esds +
                           1); // mp4box with ogg will set this to DD,
                               // mp4a has it as 0x40, mp4v has 0x20

        if (track_info->ObjectTypeIndication < 0xC0 &&
            a_v_flag < 0xA0) { // 0xC0 marks user streams; but things below that
                               // might still be wrong (like 0x6D - png)
          offset_into_esds += 5;
          track_info->max_bitrate = APar_read32(
              uint32_buffer, isofile, esds_start + offset_into_esds);
          offset_into_esds += 4;
          track_info->avg_bitrate = APar_read32(
              uint32_buffer, isofile, esds_start + offset_into_esds);
          offset_into_esds += 4;
        }
      } else {
        break;
      }

      if (APar_read8(isofile, esds_start + offset_into_esds) == 0x05) {
        offset_into_esds++;
        offset_into_esds +=
            APar_skip_filler(isofile, esds_start + offset_into_esds);

        uint8_t section5_length =
            APar_read8(isofile, esds_start + offset_into_esds);
        if ((section5_length <= section4_length || section4_length == 1) &&
            section5_length != 0) {
          track_info->section5_length = section5_length;
          offset_into_esds += 1;

          if (track_info->type_of_track & AUDIO_TRACK) {
            uint8_t packed_objID = APar_read8(
                isofile,
                esds_start + offset_into_esds); // its packed with channel, but
                                                // channel is fetched separately
            track_info->descriptor_object_typeID = packed_objID >> 3;
            offset_into_esds += 1;

            track_info->channels = (uint16_t)APar_ExtractChannelInfo(
                isofile, esds_start + offset_into_esds);

          } else if (track_info->type_of_track & VIDEO_TRACK) {
            // technically, visual_object_sequence_start_code should be tested
            // aginst 0x000001B0
            if (APar_read16(uint32_buffer,
                            isofile,
                            esds_start + offset_into_esds + 2) == 0x01B0) {
              track_info->m4v_profile =
                  APar_read8(isofile, esds_start + offset_into_esds + 2 + 2);
            }
          }
        }
        break; // uh, I've extracted the pertinent info
      }
    }
    if (offset_into_stsd > parsedAtoms[track_level_atom].AtomicLength) {
      break;
    }
  }
  if ((track_info->section5_length == 0 &&
       track_info->type_of_track & AUDIO_TRACK) ||
      track_info->channels == 0) {
    track_info->channels = APar_read16(
        uint32_buffer, isofile, parsedAtoms[track_level_atom].AtomicStart + 40);
  }
  return;
}
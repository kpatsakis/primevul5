void APar_ExtractTrackDetails(char *uint32_buffer,
                              FILE *isofile,
                              Trackage *track,
                              TrackInfo *track_info) {
  uint64_t _offset = 0;

  APar_TrackLevelInfo(track, "tkhd");
  if (APar_read8(isofile, parsedAtoms[track->track_atom].AtomicStart + 8) ==
      0) {
    if (APar_read8(isofile, parsedAtoms[track->track_atom].AtomicStart + 11) &
        1) {
      track_info->track_enabled = true;
    }
    track_info->creation_time =
        APar_read32(uint32_buffer,
                    isofile,
                    parsedAtoms[track->track_atom].AtomicStart + 12);
    track_info->modified_time =
        APar_read32(uint32_buffer,
                    isofile,
                    parsedAtoms[track->track_atom].AtomicStart + 16);
    track_info->duration =
        APar_read32(uint32_buffer,
                    isofile,
                    parsedAtoms[track->track_atom].AtomicStart + 28);
  } else {
    track_info->creation_time =
        APar_read64(uint32_buffer,
                    isofile,
                    parsedAtoms[track->track_atom].AtomicStart + 12);
    track_info->modified_time =
        APar_read64(uint32_buffer,
                    isofile,
                    parsedAtoms[track->track_atom].AtomicStart + 20);
    track_info->duration =
        APar_read64(uint32_buffer,
                    isofile,
                    parsedAtoms[track->track_atom].AtomicStart + 36);
  }

  // language code
  APar_TrackLevelInfo(track, "mdhd");
  memset(uint32_buffer, 0, 5);
  uint16_t packed_language = APar_read16(
      uint32_buffer, isofile, parsedAtoms[track->track_atom].AtomicStart + 28);
  memset(track_info->unpacked_lang, 0, 4);
  APar_UnpackLanguage(
      track_info->unpacked_lang,
      packed_language); // http://www.w3.org/WAI/ER/IG/ert/iso639.htm

  // track handler type
  APar_TrackLevelInfo(track, "hdlr");
  memset(uint32_buffer, 0, 5);
  track_info->track_type = APar_read32(
      uint32_buffer, isofile, parsedAtoms[track->track_atom].AtomicStart + 16);
  if (track_info->track_type == 0x736F756E) { // soun
    track_info->type_of_track = AUDIO_TRACK;
  } else if (track_info->track_type == 0x76696465) { // vide
    track_info->type_of_track = VIDEO_TRACK;
  }
  if (parsedAtoms[track->track_atom].AtomicLength > 34) {
    memset(track_info->track_hdlr_name, 0, sizeof(track_info->track_hdlr_name));
    APar_readX(track_info->track_hdlr_name,
               isofile,
               parsedAtoms[track->track_atom].AtomicStart + 32,
               std::min((uint64_t)sizeof(track_info->track_hdlr_name),
                        parsedAtoms[track->track_atom].AtomicLength - 32));
  }

  // codec section
  APar_TrackLevelInfo(track, "stsd");
  memset(uint32_buffer, 0, 5);
  track_info->track_codec = APar_read32(
      uint32_buffer, isofile, parsedAtoms[track->track_atom].AtomicStart + 20);

  if (track_info->type_of_track & VIDEO_TRACK) { // vide
    track_info->video_width =
        APar_read16(uint32_buffer,
                    isofile,
                    parsedAtoms[track->track_atom + 1].AtomicStart + 32);
    track_info->video_height =
        APar_read16(uint32_buffer,
                    isofile,
                    parsedAtoms[track->track_atom + 1].AtomicStart + 34);
    track_info->macroblocks =
        (track_info->video_width / 16) * (track_info->video_height / 16);

    // avc profile & level
    if (track_info->track_codec == 0x61766331 ||
        track_info->track_codec == 0x64726D69) { // avc1 or drmi
      track_info->contains_esds = false;
      APar_TrackLevelInfo(track, "avcC");
      // get avc1 profile/level; atom 'avcC' is :
      // byte 1	configurationVersion    byte 2	AVCProfileIndication byte 3
      // profile_compatibility    byte 4	AVCLevelIndication
      track_info->avc_version =
          APar_read8(isofile, parsedAtoms[track->track_atom].AtomicStart + 8);
      if (track_info->avc_version == 1) {
        track_info->profile =
            APar_read8(isofile, parsedAtoms[track->track_atom].AtomicStart + 9);
        // uint8_t profile_compatibility = APar_read8(isofile,
        // parsedAtoms[track.track_atom].AtomicStart + 10); /* is this reserved
        // ?? */
        track_info->level = APar_read8(
            isofile, parsedAtoms[track->track_atom].AtomicStart + 11);
      }

      // avc1 doesn't have a hardcoded bitrate, so calculate it (off of stsz
      // table summing) later
    } else if (track_info->track_codec == 0x73323633) { // s263
      APar_TrackLevelInfo(track, "d263");
      if (memcmp(parsedAtoms[track->track_atom].AtomicName, "d263", 4) == 0) {
        APar_Extract_d263_Info(
            uint32_buffer, isofile, track->track_atom, track_info);
      }

    } else { // mp4v
      APar_TrackLevelInfo(track, "esds");
      if (memcmp(parsedAtoms[track->track_atom].AtomicName, "esds", 4) == 0) {
        APar_Extract_esds_Info(
            uint32_buffer,
            isofile,
            track->track_atom - 1,
            track_info); // right, backtrack to the atom before 'esds' so we can
                         // offset_into_stsd++
      } else if (track_info->track_codec == 0x73323633) { // s263
        track_info->type_of_track = VIDEO_TRACK;
      } else if (track_info->track_codec == 0x73616D72 ||
                 track_info->track_codec == 0x73617762 ||
                 track_info->track_codec == 0x73617770 ||
                 track_info->track_codec ==
                     0x73766D72) { // samr, sawb, sawp & svmr
        track_info->type_of_track = AUDIO_TRACK;
      } else {
        track_info->type_of_track = OTHER_TRACK; // a 'jpeg' track will fall
                                                 // here
      }
    }

  } else if (track_info->type_of_track & AUDIO_TRACK) {
    if (track_info->track_codec == 0x73616D72 ||
        track_info->track_codec == 0x73617762 ||
        track_info->track_codec == 0x73617770 ||
        track_info->track_codec ==
            0x73766D72) { // samr,sawb, svmr (sawp doesn't contain modes)
      APar_Extract_AMR_Info(
          uint32_buffer, isofile, track->track_atom + 2, track_info);

    } else if (track_info->track_codec == 0x73657663) { // sevc
      APar_TrackLevelInfo(track, "devc");
      if (memcmp(parsedAtoms[track->track_atom].AtomicName, "devc", 4) == 0) {
        APar_Extract_devc_Info(isofile, track->track_atom, track_info);
      }

    } else if (track_info->track_codec == 0x73716370) { // sqcp
      APar_TrackLevelInfo(track, "dqcp");
      if (memcmp(parsedAtoms[track->track_atom].AtomicName, "dqcp", 4) == 0) {
        APar_Extract_devc_Info(isofile,
                               track->track_atom,
                               track_info); // its the same thing
      }

    } else if (track_info->track_codec == 0x73736D76) { // ssmv
      APar_TrackLevelInfo(track, "dsmv");
      if (memcmp(parsedAtoms[track->track_atom].AtomicName, "dsmv", 4) == 0) {
        APar_Extract_devc_Info(isofile,
                               track->track_atom,
                               track_info); // its the same thing
      }

    } else {
      APar_Extract_esds_Info(
          uint32_buffer, isofile, track->track_atom, track_info);
    }
  }

  // in case bitrate isn't found, manually determine it off of stsz summing
  if ((track_info->type_of_track & AUDIO_TRACK ||
       track_info->type_of_track & VIDEO_TRACK) &&
      track_info->avg_bitrate == 0) {
    if (track_info->track_codec == 0x616C6163) { // alac
      track_info->channels =
          APar_read16(uint32_buffer,
                      isofile,
                      parsedAtoms[track->track_atom + 1].AtomicStart + 24);
    }
  }

  APar_TrackLevelInfo(track, "stsz");
  if (memcmp(parsedAtoms[track->track_atom].AtomicName, "stsz", 4) == 0) {
    track_info->sample_aggregate =
        calcuate_sample_size(uint32_buffer, isofile, track->track_atom);
  }

  // get what exactly 'drmX' stands in for
  if (track_info->track_codec >= 0x64726D00 &&
      track_info->track_codec <= 0x64726DFF) {
    track_info->type_of_track += DRM_PROTECTED_TRACK;
    APar_TrackLevelInfo(track, "frma");
    memset(uint32_buffer, 0, 5);
    track_info->protected_codec = APar_read32(
        uint32_buffer, isofile, parsedAtoms[track->track_atom].AtomicStart + 8);
  }

  // Encoder string; occasionally, it appears under stsd for a video track; it
  // is typcally preceded by ' ' (1st char is unprintable) or 0x01B2
  if (track_info->contains_esds) {
    APar_TrackLevelInfo(track, "esds");

    // technically, user_data_start_code should be tested aginst 0x000001B2;
    // TODO: it should only be read up to section 3's length too
    _offset = APar_FindValueInAtom(
        uint32_buffer, isofile, track->track_atom, 24, 0x01B2);

    if (_offset > 0 && _offset < parsedAtoms[track->track_atom].AtomicLength) {
      _offset += 2;
      memset(track_info->encoder_name,
             0,
             parsedAtoms[track->track_atom].AtomicLength - _offset);
      APar_readX(track_info->encoder_name,
                 isofile,
                 parsedAtoms[track->track_atom].AtomicStart + _offset,
                 parsedAtoms[track->track_atom].AtomicLength - _offset);
    }
  }
  return;
}
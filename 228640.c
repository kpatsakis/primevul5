void APar_Print_TrackDetails(TrackInfo *track_info) {
  if (track_info->max_bitrate > 0 && track_info->avg_bitrate > 0) {
    fprintf(stdout, "     %.2f kbp/s", (float)track_info->avg_bitrate / 1000.0);
  } else { // some ffmpeg encodings have avg_bitrate set to 0, but an inexact
           // max_bitrate - actually, their esds seems a mess to me
#if defined(_MSC_VER)
    fprintf(stdout,
            "     %.2lf* kbp/s",
            ((double)((__int64)track_info->sample_aggregate) /
             ((double)((__int64)track_info->duration) /
              (double)((__int64)movie_info.timescale))) /
                1000.0 * 8);
    fprintf(stdout,
            "  %.3f sec",
            (float)track_info->duration / (float)movie_info.timescale);
#else
    fprintf(stdout,
            "     %.2lf* kbp/s",
            ((double)track_info->sample_aggregate /
             ((double)track_info->duration / (double)movie_info.timescale)) /
                1000.0 * 8);
    fprintf(stdout,
            "  %.3f sec",
            (float)track_info->duration / (float)movie_info.timescale);
#endif
  }

  if (track_info->track_codec == 0x6D703476) { // mp4v profile
    APar_ShowObjectProfileInfo(MP4V_TRACK, track_info);
  } else if (track_info->track_codec == 0x6D703461 ||
             track_info->protected_codec == 0x6D703461) { // mp4a profile
    APar_ShowObjectProfileInfo(AUDIO_TRACK, track_info);
  } else if (track_info->track_codec ==
             0x616C6163) { // alac - can't figure out a hardcoded bitrate either
    fprintf(
        stdout, "  Apple Lossless    channels: [%u]\n", track_info->channels);
  } else if (track_info->track_codec == 0x61766331 ||
             track_info->protected_codec == 0x61766331) {
    if (track_info->avc_version == 1) { // avc profile & level
      APar_ShowObjectProfileInfo(AVC1_TRACK, track_info);
    }
  } else if (track_info->track_codec == 0x73323633) { // s263 in 3gp
    APar_ShowObjectProfileInfo(S263_TRACK, track_info);
  } else if (track_info->track_codec == 0x73616D72 ||
             track_info->track_codec == 0x73617762 ||
             track_info->track_codec == 0x73617770 ||
             track_info->track_codec ==
                 0x73766D72) { // samr,sawb,sawp & svmr in 3gp
    track_info->type_of_track = S_AMR_TRACK;
    APar_ShowObjectProfileInfo(track_info->type_of_track, track_info);
  } else if (track_info->track_codec == 0x73657663) { // evrc in 3gp
    track_info->type_of_track = EVRC_TRACK;
    APar_ShowObjectProfileInfo(track_info->type_of_track, track_info);
  } else if (track_info->track_codec == 0x73716370) { // qcelp in 3gp
    track_info->type_of_track = QCELP_TRACK;
    APar_ShowObjectProfileInfo(track_info->type_of_track, track_info);
  } else if (track_info->track_codec == 0x73736D76) { // smv in 3gp
    track_info->type_of_track = SMV_TRACK;
    APar_ShowObjectProfileInfo(track_info->type_of_track, track_info);
  } else { // unknown everything, 0 hardcoded bitrate
    APar_ShowObjectProfileInfo(track_info->type_of_track, track_info);
    fprintf(stdout, "\n");
  }

  if (track_info->type_of_track & VIDEO_TRACK &&
      ((track_info->max_bitrate > 0 &&
        track_info->ObjectTypeIndication == 0x20) ||
       track_info->avc_version == 1 || track_info->protected_codec != 0)) {
    fprintf(stdout,
            "  %ux%u  (%" PRIu32 " macroblocks)\n",
            track_info->video_width,
            track_info->video_height,
            track_info->macroblocks);
  } else if (track_info->type_of_track & VIDEO_TRACK) {
    fprintf(stdout, "\n");
  }
  return;
}
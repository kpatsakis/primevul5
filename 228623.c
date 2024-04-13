char *secsTOtime(double seconds) {
  ap_time time_duration = {0};
  uint32_t whole_secs = (uint32_t)(seconds / 1);

  time_duration.rem_millisecs = seconds - (double)whole_secs;
  time_duration.hours = whole_secs / 3600;
  whole_secs -= time_duration.hours * 3600;
  time_duration.minutes = whole_secs / 60;
  whole_secs -= time_duration.minutes * 60;
  time_duration.seconds = whole_secs;

  static char hhmmss_time[20];
  memset(hhmmss_time, 0, 20);
  char milli[5];
  memset(milli, 0, 5);

  uint8_t time_offset = 0;
  if (time_duration.hours > 0) {
    if (time_duration.hours < 10) {
      sprintf(hhmmss_time, "0%u:", time_duration.hours);
    } else {
      sprintf(hhmmss_time, "%u:", time_duration.hours);
    }
    time_offset += 3;
  }
  if (time_duration.minutes > 0) {
    if (time_duration.minutes < 10) {
      sprintf(hhmmss_time + time_offset, "0%u:", time_duration.minutes);
    } else {
      sprintf(hhmmss_time + time_offset, "%u:", time_duration.minutes);
    }
    time_offset += 3;
  } else {
    memcpy(hhmmss_time + time_offset, "0:", 2);
    time_offset += 2;
  }
  if (time_duration.seconds > 0) {
    if (time_duration.seconds < 10) {
      sprintf(hhmmss_time + time_offset, "0%u", time_duration.seconds);
    } else {
      sprintf(hhmmss_time + time_offset, "%u", time_duration.seconds);
    }
    time_offset += 2;
  } else {
    memcpy(hhmmss_time + time_offset, "0.", 2);
    time_offset += 1;
  }

  sprintf(
      milli,
      "%.2lf",
      time_duration.rem_millisecs); // sprintf the double float into a new
                                    // string because I don't know if there is a
                                    // way to print without a leading zero
  memcpy(hhmmss_time + time_offset, milli + 1, 3);

  return *&hhmmss_time;
}
char *FoFiType1C::getString(int sid, char *buf, GBool *ok) {
  Type1CIndexVal val;
  int n;

  if (sid < 0) {
    buf[0] = '\0';
  } else if (sid < 391) {
    strcpy(buf, fofiType1CStdStrings[sid]);
  } else {
    sid -= 391;
    getIndexVal(&stringIdx, sid, &val, ok);
    if (*ok) {
      if ((n = val.len) > 255) {
	n = 255;
      }
      strncpy(buf, (char *)&file[val.pos], n);
      buf[n] = '\0';
    } else {
      buf[0] = '\0';
    }
  }
  return buf;
}

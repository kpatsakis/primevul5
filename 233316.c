CURLcode telrcv(struct Curl_easy *data,
                const unsigned char *inbuf, /* Data received from socket */
                ssize_t count)              /* Number of bytes received */
{
  unsigned char c;
  CURLcode result;
  int in = 0;
  int startwrite = -1;
  struct TELNET *tn = data->req.p.telnet;

#define startskipping()                                       \
  if(startwrite >= 0) {                                       \
    result = Curl_client_write(data,                          \
                               CLIENTWRITE_BODY,              \
                               (char *)&inbuf[startwrite],    \
                               in-startwrite);                \
    if(result)                                                \
      return result;                                          \
  }                                                           \
  startwrite = -1

#define writebyte() \
    if(startwrite < 0) \
      startwrite = in

#define bufferflush() startskipping()

  while(count--) {
    c = inbuf[in];

    switch(tn->telrcv_state) {
    case CURL_TS_CR:
      tn->telrcv_state = CURL_TS_DATA;
      if(c == '\0') {
        startskipping();
        break;   /* Ignore \0 after CR */
      }
      writebyte();
      break;

    case CURL_TS_DATA:
      if(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        break;
      }
      else if(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      break;

    case CURL_TS_IAC:
    process_iac:
      DEBUGASSERT(startwrite < 0);
      switch(c) {
      case CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        break;
      case CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        break;
      case CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        break;
      case CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        break;
      case CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        break;
      case CURL_IAC:
        tn->telrcv_state = CURL_TS_DATA;
        writebyte();
        break;
      case CURL_DM:
      case CURL_NOP:
      case CURL_GA:
      default:
        tn->telrcv_state = CURL_TS_DATA;
        printoption(data, "RCVD", CURL_IAC, c);
        break;
      }
      break;

      case CURL_TS_WILL:
        printoption(data, "RCVD", CURL_WILL, c);
        tn->please_negotiate = 1;
        rec_will(data, c);
        tn->telrcv_state = CURL_TS_DATA;
        break;

      case CURL_TS_WONT:
        printoption(data, "RCVD", CURL_WONT, c);
        tn->please_negotiate = 1;
        rec_wont(data, c);
        tn->telrcv_state = CURL_TS_DATA;
        break;

      case CURL_TS_DO:
        printoption(data, "RCVD", CURL_DO, c);
        tn->please_negotiate = 1;
        rec_do(data, c);
        tn->telrcv_state = CURL_TS_DATA;
        break;

      case CURL_TS_DONT:
        printoption(data, "RCVD", CURL_DONT, c);
        tn->please_negotiate = 1;
        rec_dont(data, c);
        tn->telrcv_state = CURL_TS_DATA;
        break;

      case CURL_TS_SB:
        if(c == CURL_IAC)
          tn->telrcv_state = CURL_TS_SE;
        else
          CURL_SB_ACCUM(tn, c);
        break;

      case CURL_TS_SE:
        if(c != CURL_SE) {
          if(c != CURL_IAC) {
            /*
             * This is an error.  We only expect to get "IAC IAC" or "IAC SE".
             * Several things may have happened.  An IAC was not doubled, the
             * IAC SE was left off, or another option got inserted into the
             * suboption are all possibilities.  If we assume that the IAC was
             * not doubled, and really the IAC SE was left off, we could get
             * into an infinite loop here.  So, instead, we terminate the
             * suboption, and process the partial suboption if we can.
             */
            CURL_SB_ACCUM(tn, CURL_IAC);
            CURL_SB_ACCUM(tn, c);
            tn->subpointer -= 2;
            CURL_SB_TERM(tn);

            printoption(data, "In SUBOPTION processing, RCVD", CURL_IAC, c);
            suboption(data);   /* handle sub-option */
            tn->telrcv_state = CURL_TS_IAC;
            goto process_iac;
          }
          CURL_SB_ACCUM(tn, c);
          tn->telrcv_state = CURL_TS_SB;
        }
        else {
          CURL_SB_ACCUM(tn, CURL_IAC);
          CURL_SB_ACCUM(tn, CURL_SE);
          tn->subpointer -= 2;
          CURL_SB_TERM(tn);
          suboption(data);   /* handle sub-option */
          tn->telrcv_state = CURL_TS_DATA;
        }
        break;
    }
    ++in;
  }
  bufferflush();
  return CURLE_OK;
}
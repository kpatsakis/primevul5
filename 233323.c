void rec_wont(struct Curl_easy *data, int option)
{
  struct TELNET *tn = data->req.p.telnet;
  switch(tn->him[option]) {
  case CURL_NO:
    /* Already disabled */
    break;

  case CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(data, CURL_DONT, option);
    break;

  case CURL_WANTNO:
    switch(tn->himq[option]) {
    case CURL_EMPTY:
      tn->him[option] = CURL_NO;
      break;

    case CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DO, option);
      break;
    }
    break;

  case CURL_WANTYES:
    switch(tn->himq[option]) {
    case CURL_EMPTY:
      tn->him[option] = CURL_NO;
      break;
    case CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_EMPTY;
      break;
    }
    break;
  }
}
static void negotiate(struct Curl_easy *data)
{
  int i;
  struct TELNET *tn = data->req.p.telnet;

  for(i = 0; i < CURL_NTELOPTS; i++) {
    if(i == CURL_TELOPT_ECHO)
      continue;

    if(tn->us_preferred[i] == CURL_YES)
      set_local_option(data, i, CURL_YES);

    if(tn->him_preferred[i] == CURL_YES)
      set_remote_option(data, i, CURL_YES);
  }
}
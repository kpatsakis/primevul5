static CURLcode telnet_done(struct Curl_easy *data,
                            CURLcode status, bool premature)
{
  struct TELNET *tn = data->req.p.telnet;
  (void)status; /* unused */
  (void)premature; /* not used */

  if(!tn)
    return CURLE_OK;

  curl_slist_free_all(tn->telnet_vars);
  tn->telnet_vars = NULL;

  Curl_safefree(data->req.p.telnet);

  return CURLE_OK;
}
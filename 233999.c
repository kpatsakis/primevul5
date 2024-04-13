url_free (struct url *url)
{
  if (url)
    {
      if (url->idn_allocated) {
        idn_free (url->host);      /* A dummy if !defined(ENABLE_IRI) */
        url->host = NULL;
      }
      else
        xfree (url->host);

      xfree (url->path);
      xfree (url->url);

      xfree (url->params);
      xfree (url->query);
      xfree (url->fragment);
      xfree (url->user);
      xfree (url->passwd);

      xfree (url->dir);
      xfree (url->file);

      xfree (url);
    }
}
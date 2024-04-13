test_append_uri_pathel(void)
{
  unsigned i;
  static const struct {
    const char *original_url;
    const char *input;
    bool escaped;
    const char *expected_result;
  } test_array[] = {
    { "http://www.yoyodyne.com/path/", "somepage.html", false, "http://www.yoyodyne.com/path/somepage.html" },
  };

  for (i = 0; i < countof(test_array); ++i)
    {
      struct growable dest;
      const char *p = test_array[i].input;

      memset (&dest, 0, sizeof (dest));

      append_string (test_array[i].original_url, &dest);
      append_uri_pathel (p, p + strlen(p), test_array[i].escaped, &dest);

      mu_assert ("test_append_uri_pathel: wrong result",
                 strcmp (dest.base, test_array[i].expected_result) == 0);
      xfree (dest.base);
    }

  return NULL;
}
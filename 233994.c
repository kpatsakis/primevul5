test_are_urls_equal(void)
{
  unsigned i;
  static const struct {
    const char *url1;
    const char *url2;
    bool expected_result;
  } test_array[] = {
    { "http://www.adomain.com/apath/", "http://www.adomain.com/apath/",       true },
    { "http://www.adomain.com/apath/", "http://www.adomain.com/anotherpath/", false },
    { "http://www.adomain.com/apath/", "http://www.anotherdomain.com/path/",  false },
    { "http://www.adomain.com/~path/", "http://www.adomain.com/%7epath/",     true },
    { "http://www.adomain.com/longer-path/", "http://www.adomain.com/path/",  false },
    { "http://www.adomain.com/path%2f", "http://www.adomain.com/path/",       false },
  };

  for (i = 0; i < countof(test_array); ++i)
    {
      mu_assert ("test_are_urls_equal: wrong result",
                 are_urls_equal (test_array[i].url1, test_array[i].url2) == test_array[i].expected_result);
    }

  return NULL;
}
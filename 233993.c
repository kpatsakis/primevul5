test_path_simplify (void)
{
  static const struct {
    const char *test, *result;
    enum url_scheme scheme;
    bool should_modify;
  } tests[] = {
    { "",                       "",             SCHEME_HTTP, false },
    { ".",                      "",             SCHEME_HTTP, true },
    { "./",                     "",             SCHEME_HTTP, true },
    { "..",                     "",             SCHEME_HTTP, true },
    { "../",                    "",             SCHEME_HTTP, true },
    { "..",                     "..",           SCHEME_FTP,  false },
    { "../",                    "../",          SCHEME_FTP,  false },
    { "foo",                    "foo",          SCHEME_HTTP, false },
    { "foo/bar",                "foo/bar",      SCHEME_HTTP, false },
    { "foo///bar",              "foo///bar",    SCHEME_HTTP, false },
    { "foo/.",                  "foo/",         SCHEME_HTTP, true },
    { "foo/./",                 "foo/",         SCHEME_HTTP, true },
    { "foo./",                  "foo./",        SCHEME_HTTP, false },
    { "foo/../bar",             "bar",          SCHEME_HTTP, true },
    { "foo/../bar/",            "bar/",         SCHEME_HTTP, true },
    { "foo/bar/..",             "foo/",         SCHEME_HTTP, true },
    { "foo/bar/../x",           "foo/x",        SCHEME_HTTP, true },
    { "foo/bar/../x/",          "foo/x/",       SCHEME_HTTP, true },
    { "foo/..",                 "",             SCHEME_HTTP, true },
    { "foo/../..",              "",             SCHEME_HTTP, true },
    { "foo/../../..",           "",             SCHEME_HTTP, true },
    { "foo/../../bar/../../baz", "baz",         SCHEME_HTTP, true },
    { "foo/../..",              "..",           SCHEME_FTP,  true },
    { "foo/../../..",           "../..",        SCHEME_FTP,  true },
    { "foo/../../bar/../../baz", "../../baz",   SCHEME_FTP,  true },
    { "a/b/../../c",            "c",            SCHEME_HTTP, true },
    { "./a/../b",               "b",            SCHEME_HTTP, true }
  };
  unsigned i;

  for (i = 0; i < countof (tests); i++)
    {
      const char *message;
      const char *test = tests[i].test;
      const char *expected_result = tests[i].result;
      enum url_scheme scheme = tests[i].scheme;
      bool  expected_change = tests[i].should_modify;

      message = run_test (test, expected_result, scheme, expected_change);
      if (message) return message;
    }
  return NULL;
}
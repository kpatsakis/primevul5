run_test (const char *test, const char *expected_result, enum url_scheme scheme,
          bool expected_change)
{
  char *test_copy = xstrdup (test);
  bool modified = path_simplify (scheme, test_copy);

  if (0 != strcmp (test_copy, expected_result))
    {
      printf ("Failed path_simplify(\"%s\"): expected \"%s\", got \"%s\".\n",
              test, expected_result, test_copy);
      mu_assert ("", 0);
    }
  if (modified != expected_change)
    {
      if (expected_change)
        printf ("Expected modification with path_simplify(\"%s\").\n",
                test);
      else
        printf ("Expected no modification with path_simplify(\"%s\").\n",
                test);
    }
  xfree (test_copy);
  mu_assert ("", modified == expected_change);
  return NULL;
}
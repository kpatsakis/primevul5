setmatcher (char const *m)
{
  static char const *matcher;
  unsigned int i;

  if (!m)
    {
      compile = matchers[0].compile;
      execute = matchers[0].execute;
      if (!matchers[1].name)
        matcher = matchers[0].name;
    }

  else if (matcher)
    {
      if (matcher && STREQ (matcher, m))
        ;

      else if (!matchers[1].name)
        error (EXIT_TROUBLE, 0, _("%s can only use the %s pattern syntax"),
               program_name, matcher);
      else
        error (EXIT_TROUBLE, 0, _("conflicting matchers specified"));
    }

  else
    {
      for (i = 0; matchers[i].name; i++)
        if (STREQ (m, matchers[i].name))
          {
            compile = matchers[i].compile;
            execute = matchers[i].execute;
            matcher = m;
            return;
          }

      error (EXIT_TROUBLE, 0, _("invalid matcher %s"), m);
    }
}
print_length (wgint size, wgint start, bool authoritative)
{
  logprintf (LOG_VERBOSE, _("Length: %s"), number_to_static_string (size));
  if (size >= 1024)
    logprintf (LOG_VERBOSE, " (%s)", human_readable (size, 10, 1));
  if (start > 0)
    {
      if (size - start >= 1024)
        logprintf (LOG_VERBOSE, _(", %s (%s) remaining"),
                   number_to_static_string (size - start),
                   human_readable (size - start, 10, 1));
      else
        logprintf (LOG_VERBOSE, _(", %s remaining"),
                   number_to_static_string (size - start));
    }
  logputs (LOG_VERBOSE, !authoritative ? _(" (unauthoritative)\n") : "\n");
}

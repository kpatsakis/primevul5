usage (int status)
{
  if (status != 0)
    {
      fprintf (stderr, _("Usage: %s [OPTION]... PATTERN [FILE]...\n"),
               program_name);
      fprintf (stderr, _("Try '%s --help' for more information.\n"),
               program_name);
    }
  else
    {
      printf (_("Usage: %s [OPTION]... PATTERN [FILE]...\n"), program_name);
      printf (_("\
Search for PATTERN in each FILE or standard input.\n"));
      fputs (_(before_options), stdout);
      printf (_("\
Example: %s -i 'hello world' menu.h main.c\n\
\n\
Regexp selection and interpretation:\n"), program_name);
      if (matchers[1].name)
        printf (_("\
  -E, --extended-regexp     PATTERN is an extended regular expression (ERE)\n\
  -F, --fixed-strings       PATTERN is a set of newline-separated fixed strings\n\
  -G, --basic-regexp        PATTERN is a basic regular expression (BRE)\n\
  -P, --perl-regexp         PATTERN is a Perl regular expression\n"));
  /* -X is undocumented on purpose. */
      printf (_("\
  -e, --regexp=PATTERN      use PATTERN for matching\n\
  -f, --file=FILE           obtain PATTERN from FILE\n\
  -i, --ignore-case         ignore case distinctions\n\
  -w, --word-regexp         force PATTERN to match only whole words\n\
  -x, --line-regexp         force PATTERN to match only whole lines\n\
  -z, --null-data           a data line ends in 0 byte, not newline\n"));
      printf (_("\
\n\
Miscellaneous:\n\
  -s, --no-messages         suppress error messages\n\
  -v, --invert-match        select non-matching lines\n\
  -V, --version             print version information and exit\n\
      --help                display this help and exit\n\
      --mmap                deprecated no-op; evokes a warning\n"));
      printf (_("\
\n\
Output control:\n\
  -m, --max-count=NUM       stop after NUM matches\n\
  -b, --byte-offset         print the byte offset with output lines\n\
  -n, --line-number         print line number with output lines\n\
      --line-buffered       flush output on every line\n\
  -H, --with-filename       print the file name for each match\n\
  -h, --no-filename         suppress the file name prefix on output\n\
      --label=LABEL         use LABEL as the standard input file name prefix\n\
"));
      printf (_("\
  -o, --only-matching       show only the part of a line matching PATTERN\n\
  -q, --quiet, --silent     suppress all normal output\n\
      --binary-files=TYPE   assume that binary files are TYPE;\n\
                            TYPE is `binary', `text', or `without-match'\n\
  -a, --text                equivalent to --binary-files=text\n\
"));
      printf (_("\
  -I                        equivalent to --binary-files=without-match\n\
  -d, --directories=ACTION  how to handle directories;\n\
                            ACTION is `read', `recurse', or `skip'\n\
  -D, --devices=ACTION      how to handle devices, FIFOs and sockets;\n\
                            ACTION is `read' or `skip'\n\
  -R, -r, --recursive       equivalent to --directories=recurse\n\
"));
      printf (_("\
      --include=FILE_PATTERN  search only files that match FILE_PATTERN\n\
      --exclude=FILE_PATTERN  skip files and directories matching FILE_PATTERN\n\
      --exclude-from=FILE   skip files matching any file pattern from FILE\n\
      --exclude-dir=PATTERN  directories that match PATTERN will be skipped.\n\
"));
      printf (_("\
  -L, --files-without-match  print only names of FILEs containing no match\n\
  -l, --files-with-matches  print only names of FILEs containing matches\n\
  -c, --count               print only a count of matching lines per FILE\n\
  -T, --initial-tab         make tabs line up (if needed)\n\
  -Z, --null                print 0 byte after FILE name\n"));
      printf (_("\
\n\
Context control:\n\
  -B, --before-context=NUM  print NUM lines of leading context\n\
  -A, --after-context=NUM   print NUM lines of trailing context\n\
  -C, --context=NUM         print NUM lines of output context\n\
"));
      printf (_("\
  -NUM                      same as --context=NUM\n\
      --color[=WHEN],\n\
      --colour[=WHEN]       use markers to highlight the matching strings;\n\
                            WHEN is `always', `never', or `auto'\n\
  -U, --binary              do not strip CR characters at EOL (MSDOS/Windows)\n\
  -u, --unix-byte-offsets   report offsets as if CRs were not there\n\
                            (MSDOS/Windows)\n\
\n"));
      fputs (_(after_options), stdout);
      printf (_("\
When FILE is -, read standard input.  With no FILE, read . if a command-line\n\
-r is given, - otherwise.  If fewer than two FILEs are given, assume -h.\n\
Exit status is 0 if any line is selected, 1 otherwise;\n\
if any error occurs and -q is not given, the exit status is 2.\n"));
      printf (_("\nReport bugs to: %s\n"), PACKAGE_BUGREPORT);
      printf (_("GNU Grep home page: <%s>\n"),
              "http://www.gnu.org/software/grep/");
      fputs (_("General help using GNU software: <http://www.gnu.org/gethelp/>\n"),
             stdout);

    }
  exit (status);
}
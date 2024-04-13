main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/socket-client/happy-eyeballs/slow", test_happy_eyeballs);
  g_test_add_func ("/socket-client/happy-eyeballs/cancellation", test_happy_eyeballs_cancel);

  return g_test_run ();
}
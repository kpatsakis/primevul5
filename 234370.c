timed_pause(double sleep_time)
{
#if defined(HAVE_USLEEP) && defined(USE_MOUSE) && !defined(_WIN32)
    if (term->waitforinput)		/* If the terminal supports it */
	while (sleep_time > 0.05) {	/* we poll 20 times a second */
	    usleep(50000);		/* Sleep for 50 msec */
	    check_for_mouse_events();
	    sleep_time -= 0.05;
	}
    usleep((useconds_t)(sleep_time * 1e6));
    check_for_mouse_events();
#else
    GP_SLEEP(sleep_time);
#endif
}
static void die_initial_contact(int unexpected)
{
	if (unexpected)
		die(_("The remote end hung up upon initial contact"));
	else
		die(_("Could not read from remote repository.\n\n"
		      "Please make sure you have the correct access rights\n"
		      "and the repository exists."));
}
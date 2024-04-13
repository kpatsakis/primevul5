static int headerMatchLocale(const char *td, const char *l, const char *le)
{
    const char *fe;

    /* First try a complete match. */
    if (strlen(td) == (le-l) && rstreqn(td, l, (le - l)))
	return 1;

    /* Next, try stripping optional dialect and matching.  */
    for (fe = l; fe < le && *fe != '@'; fe++)
	{};
    if (fe < le && rstreqn(td, l, (fe - l)))
	return 1;

    /* Next, try stripping optional codeset and matching.  */
    for (fe = l; fe < le && *fe != '.'; fe++)
	{};
    if (fe < le && rstreqn(td, l, (fe - l)))
	return 1;

    /* Finally, try stripping optional country code and matching. */
    for (fe = l; fe < le && *fe != '_'; fe++)
	{};
    if (fe < le && rstreqn(td, l, (fe - l)))
	return 2;

    return 0;
}
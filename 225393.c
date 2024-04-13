void CServer::InitRconPasswordIfUnset()
{
	if(m_RconPasswordSet)
	{
		return;
	}

	static const char VALUES[] = "ABCDEFGHKLMNPRSTUVWXYZabcdefghjkmnopqt23456789";
	static const size_t NUM_VALUES = sizeof(VALUES) - 1; // Disregard the '\0'.
	static const size_t PASSWORD_LENGTH = 6;
	dbg_assert(NUM_VALUES * NUM_VALUES >= 2048, "need at least 2048 possibilities for 2-character sequences");
	// With 6 characters, we get a password entropy of log(2048) * 6/2 = 33bit.

	dbg_assert(PASSWORD_LENGTH % 2 == 0, "need an even password length");
	unsigned short aRandom[PASSWORD_LENGTH / 2];
	char aRandomPassword[PASSWORD_LENGTH+1];
	aRandomPassword[PASSWORD_LENGTH] = 0;

	secure_random_fill(aRandom, sizeof(aRandom));
	for(size_t i = 0; i < PASSWORD_LENGTH / 2; i++)
	{
		unsigned short RandomNumber = aRandom[i] % 2048;
		aRandomPassword[2 * i + 0] = VALUES[RandomNumber / NUM_VALUES];
		aRandomPassword[2 * i + 1] = VALUES[RandomNumber % NUM_VALUES];
	}

	str_copy(Config()->m_SvRconPassword, aRandomPassword, sizeof(Config()->m_SvRconPassword));
	m_GeneratedRconPassword = 1;
}
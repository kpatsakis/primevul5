static bool nfs41_match_stateid(const nfs4_stateid *s1,
		const nfs4_stateid *s2)
{
	if (s1->type != s2->type)
		return false;

	if (memcmp(s1->other, s2->other, sizeof(s1->other)) != 0)
		return false;

	if (s1->seqid == s2->seqid)
		return true;

	return s1->seqid == 0 || s2->seqid == 0;
}
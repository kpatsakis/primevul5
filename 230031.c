static bool nfs4_match_stateid(const nfs4_stateid *s1,
		const nfs4_stateid *s2)
{
	return nfs4_stateid_match(s1, s2);
}
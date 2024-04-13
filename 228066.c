int main(int argc, char *argv[])
{
	struct libmnt_test tss[] = {
		{ "--append", test_append, "<optstr> <name> [<value>]  append value to optstr" },
		{ "--prepend",test_prepend,"<optstr> <name> [<value>]  prepend value to optstr" },
		{ "--set",    test_set,    "<optstr> <name> [<value>]  (un)set value" },
		{ "--get",    test_get,    "<optstr> <name>            search name in optstr" },
		{ "--remove", test_remove, "<optstr> <name>            remove name in optstr" },
		{ "--dedup",  test_dedup,  "<optstr> <name>            deduplicate name in optstr" },
		{ "--split",  test_split,  "<optstr>                   split into FS, VFS and userspace" },
		{ "--flags",  test_flags,  "<optstr>                   convert options to MS_* flags" },
		{ "--apply",  test_apply,  "--{linux,user} <optstr> <mask>    apply mask to optstr" },
		{ "--fix",    test_fix,    "<optstr>                   fix uid=, gid=, user, and context=" },

		{ NULL }
	};
	return  mnt_run_test(tss, argc, argv);
}
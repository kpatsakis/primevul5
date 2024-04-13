static bool new_idmap_permitted(const struct file *file, 
				struct user_namespace *ns, int cap_setid,
				struct uid_gid_map *new_map)
{
	/* Allow mapping to your own filesystem ids */
	if ((new_map->nr_extents == 1) && (new_map->extent[0].count == 1)) {
 		u32 id = new_map->extent[0].lower_first;
 		if (cap_setid == CAP_SETUID) {
 			kuid_t uid = make_kuid(ns->parent, id);
			if (uid_eq(uid, current_fsuid()))
 				return true;
 		}
 		else if (cap_setid == CAP_SETGID) {
 			kgid_t gid = make_kgid(ns->parent, id);
			if (gid_eq(gid, current_fsgid()))
 				return true;
 		}
 	}

	/* Allow anyone to set a mapping that doesn't require privilege */
	if (!cap_valid(cap_setid))
		return true;

	/* Allow the specified ids if we have the appropriate capability
	 * (CAP_SETUID or CAP_SETGID) over the parent user namespace.
	 * And the opener of the id file also had the approprpiate capability.
	 */
	if (ns_capable(ns->parent, cap_setid) &&
	    file_ns_capable(file, ns->parent, cap_setid))
		return true;

	return false;
}

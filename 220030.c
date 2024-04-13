static int hdrchkTagType(rpm_tag_t tag, rpm_tagtype_t type)
{
    rpmTagType t = rpmTagGetTagType(tag);
    if (t == type)
	return 0;

    /* Permit unknown tags for forward compatibility */
    if (t == RPM_NULL_TYPE)
	return 0;

    /* Some string tags harmlessly disagree on the exact type */
    if (rpmTagGetClass(tag) == RPM_STRING_CLASS &&
	    (rpmTagTypeGetClass(type) == RPM_STRING_CLASS))
	return 0;

    /* Known tag with mismatching type, bad bad bad. */
    return 1;
}
static const struct attr_desc *find_attr_desc(unsigned attr_type)
{
    struct attr_desc *desc;

    /* Check that attr_desc array is valid */
    pj_assert(PJ_ARRAY_SIZE(mandatory_attr_desc)==
	      PJ_STUN_ATTR_END_MANDATORY_ATTR+1);
    pj_assert(mandatory_attr_desc[PJ_STUN_ATTR_END_MANDATORY_ATTR].decode_attr
	      == NULL);
    pj_assert(mandatory_attr_desc[PJ_STUN_ATTR_USE_CANDIDATE].decode_attr 
	      == &decode_empty_attr);
    pj_assert(PJ_ARRAY_SIZE(extended_attr_desc) ==
	      PJ_STUN_ATTR_END_EXTENDED_ATTR-PJ_STUN_ATTR_START_EXTENDED_ATTR);

    if (attr_type < PJ_STUN_ATTR_END_MANDATORY_ATTR)
	desc = &mandatory_attr_desc[attr_type];
    else if (attr_type >= PJ_STUN_ATTR_START_EXTENDED_ATTR &&
	     attr_type < PJ_STUN_ATTR_END_EXTENDED_ATTR)
	desc = &extended_attr_desc[attr_type-PJ_STUN_ATTR_START_EXTENDED_ATTR];
    else
	return NULL;

    return desc->decode_attr == NULL ? NULL : desc;
}
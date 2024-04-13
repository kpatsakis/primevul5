mptcp_map_relssn_to_rawdsn(mptcp_dss_mapping_t *mapping, guint32 relssn, guint64 *dsn)
{
    if( (relssn < mapping->ssn_low) || (relssn > mapping->ssn_high)) {
        return FALSE;
    }

    *dsn = mapping->rawdsn + (relssn - mapping->ssn_low);
    return TRUE;
}
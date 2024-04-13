static int cypress_compute_slot(struct mt_application *application,
				struct mt_usages *slot)
{
	if (*slot->contactid != 0 || application->num_received == 0)
		return *slot->contactid;
	else
		return -1;
}
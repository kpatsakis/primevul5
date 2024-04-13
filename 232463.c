static void mt_post_parse_default_settings(struct mt_device *td,
					   struct mt_application *app)
{
	__s32 quirks = app->quirks;

	/* unknown serial device needs special quirks */
	if (list_is_singular(&app->mt_usages)) {
		quirks |= MT_QUIRK_ALWAYS_VALID;
		quirks &= ~MT_QUIRK_NOT_SEEN_MEANS_UP;
		quirks &= ~MT_QUIRK_VALID_IS_INRANGE;
		quirks &= ~MT_QUIRK_VALID_IS_CONFIDENCE;
		quirks &= ~MT_QUIRK_CONTACT_CNT_ACCURATE;
	}

	app->quirks = quirks;
}
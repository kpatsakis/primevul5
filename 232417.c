static void mt_post_parse(struct mt_device *td, struct mt_application *app)
{
	if (!app->have_contact_count)
		app->quirks &= ~MT_QUIRK_CONTACT_CNT_ACCURATE;
}
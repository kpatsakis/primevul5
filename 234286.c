static void characterDataHandler(void *userData, const char *s, int length)
{
    userdata_t *ud = (userdata_t *) userData;

    if (!ud->listen)
	return;

    if (ud->compositeReadState) {
	agxbput_n(&ud->composite_buffer, (char *) s, length);
	return;
    }

    agxbput_n(&ud->xml_attr_value, (char *) s, length);
}
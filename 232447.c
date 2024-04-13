static void mt_fix_const_field(struct hid_field *field, unsigned int usage)
{
	if (field->usage[0].hid != usage ||
	    !(field->flags & HID_MAIN_ITEM_CONSTANT))
		return;

	field->flags &= ~HID_MAIN_ITEM_CONSTANT;
	field->flags |= HID_MAIN_ITEM_VARIABLE;
}
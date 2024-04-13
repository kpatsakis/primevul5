static inline int __must_check hid_parse(struct hid_device *hdev)
{
	return hid_open_report(hdev);
}
static inline void bs(struct vc_data *vc)
{
	if (vc->vc_x) {
		vc->vc_pos -= 2;
		vc->vc_x--;
		vc->vc_need_wrap = 0;
		notify_write(vc, '\b');
	}
}
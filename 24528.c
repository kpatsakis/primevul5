void _ma_report_progress(HA_CHECK *param, ulonglong progress,
                         ulonglong max_progress)
{
  thd_progress_report((THD*)param->thd,
                      progress + max_progress * param->stage,
                      max_progress * param->max_stage);
}
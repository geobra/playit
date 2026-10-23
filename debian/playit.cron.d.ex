#
# Regular cron jobs for the playit package.
#
0 4	* * *	root	[ -x /usr/bin/playit_maintenance ] && /usr/bin/playit_maintenance

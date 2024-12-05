#
# Regular cron jobs for the libitute164 package
#
0 4	* * *	root	[ -x /usr/bin/libitute164_maintenance ] && /usr/bin/libitute164_maintenance

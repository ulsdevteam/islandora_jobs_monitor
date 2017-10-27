# Islandora Jobs Stack Monitor

To gather and report health and job processing for relevant jobs servers.

This module should be installed on each of the gearman worker servers as well as the gearman master server and part of the code can also be installed on the Fedora server in order to send the health values back to the underlying database.

Since this module only works with gearman (and the islandora_jobs module), but **not all servers** in the stack have that module installed, the dependency on that module is not enforced.

## Configuration
The configuration is slightly complex and is made up of three primary pieces.
1. The uls-tuque.ini configuration file.
2. The `servers` table values.
3. the CRON jobs on each server.

### The islandora_jobs_monitor.ini configuration file
The project contains a /cron/islandora_jobs_monitor.ini file that should be configured to provide the cricital information that is needed by the cron job in order to run.  If any one of the values are wrong, the `host_server_health` table can not be updated.  The critical values are all in the `islandora_jobs_monitor` and `mysql` sections.  An obfuscated version of this file is below for reference.

```[islandora_jobs_monitor]
server_id = 4
server_monitor = http://***.********.edu/
...

[mysql]
jobs_monitor_database = islandora_jobs_monitor
database = gearman_04
username = drupal
password = ****************
host = ***********
```

**Note:** *until a new field is added to the servers table to track whether or not the given server is a fedora server, any server that is a fedora server needs to have a server_id value greater than 100.*

Other servers could be configured to be included in this as well (with the connections opened to PUT data from the servers over port 80, or via direct mysql commands).

### The CRON jobs
On various machines, the required CRON jobs are different.

The "master" machine that runs the `gearmand` service would run two CRON jobs.  The first one runs once an hour to send the last 100 lines of the apache error_log to the `host_error_log` table.  The other CRON job would run every 5 minutes to report the health metrics.  These metrics are **CPU Usage %**, **Memory Usage %**, and **Disk usage %** (for /ingest/tmp partition). 
**Root crontab**:
```
# copy the last 100 lines of error log to temp file that apache can access.  this is for the islandora_jobs_monitor.
*/58 * * * * tail -n100 /var/log/httpd/error_log > /tmp/error_log_last100.txt;chown bgilling:ulssysdev /tmp/error_log_last100.txt
```

**Apache crontab** : via  `sudo crontab -u apache -e`
```
# To calculate the server health - for islandora_jobs_monitor module.
*/5 * * * * /usr/bin/php /var/www/html/drupal_gamera_sandbox/sites/all/modules/islandora_jobs_monitor/cron/report_serverhealth.php
```

On a gearman worker machine, there would be two CRON jobs.  The first one runs once an hour to send the last 100 lines of the apache error_log to the `host_error_log` table.  The other CRON job would run every 5 minutes to report the health metrics.  These metrics are **CPU Usage %**, **Memory Usage %**, and **Disk usage %** (for the main partition).
**Root crontab**:
```
# copy the last 100 lines of error log to temp file that apache can access.
*/58 * * * * tail -n100 /var/log/httpd/error_log > /tmp/error_log_last100.txt;chown bgilling:ulssysdev /tmp/error_log_last100.txt
```
**Apache crontab** : via `sudo crontab -u apache -e`
```
*/5 * * * * /usr/bin/php /var/www/html/drupal7/sites/all/modules/islandora_jobs_monitor/cron/report_serverhealth.php
```

The fedora machine does not have an apache error log, so it would only have one CRON job that runs every 5 minutes to report the health metrics.  These metrics are **CPU Usage %**, **Memory Usage %**, and **Disk usage %** (relative to the fedora datastore).
```
# To get the server health metrics to report back to islandora_jobs_monitor
*/5 * * * * /usr/bin/php /opt/islandora_cron/report_serverhealth_fedora.php
```


### Database Information
The new `islandora_jobs_monitor` database is needed to store the info and server health statistics for the various servers that make up the components of the stack.
##### `servers`
This stores the general info for each server.  The ip_address values below have been obfuscated.

server_id | hostname | ip_address | is_worker | name | master_command 
--- | --- | --- | ---:| --- | --- 
0|pa-staff-01|???.???.171.66|0|Gamera
1|pa-gmworker-01|???.???.171.75|1|gmworker-01
2|pa-gmworker-02|???.???.171.78|1|gmworker-02|clear_tmp
3|pa-gmworker-03|???.???.171.79|1|gmworker-03
4|pa-gmworker-04|???.???.171.80|1|gmworker-04
100|pa-fed-staff-01|???.???.230.34|0|Fedora

##### `host_server_health`
This table stores the health records for each server.  The CRON process will either populate this, or sent a http PUT to the [islandora_jobs_monitor] "server_monitor" value (see section on uls-tuque.ini above) server which will save the values.

server_id | timestamp | cpu_percentage | memory_percentage | error_log_errors_in_last_100 | disk_used_pct
--- | --- | ---:| ---:| ---:| ---: 
0 | 2017-10-27 14:10:06 | 2.54 | 0.973 | 3 | 32
1 | 2017-10-27 14:10:07 | 84.4 | 0.351 | 90 | 10
2 | 2017-10-27 14:10:06 | 95.05 | 0.291 | 0 | 11
3 | 2017-10-27 14:10:07 | 90.45 | 0.294 | 0 | 11
4 | 2017-10-27 14:10:06 | 93.66 | 0.251 | 0 | 10
100 | 2017-10-27 14:10:07 | 27.27 | 25.0987 | 0 | 74

##### `host_error_log`
This table stores the last 100 lines of each server's apache error_log file.

##### `host_gearman_job`
This table stores the jobs that are currently being performed on any given gearman worker machine.

## Maintainers/Sponsors
Current maintainers:

* [Brian Gillingham](https://github.com/bgilling)
* [ULS Development Team](https://github.com/ulsdevteam)

## Development

If you would like to contribute to this module, please check out [CONTRIBUTING.md](CONTRIBUTING.md). In addition, we have helpful [Documentation for Developers](https://github.com/Islandora/islandora/wiki#wiki-documentation-for-developers) info, as well as our [Developers](http://islandora.ca/developers) section on the [Islandora.ca](http://islandora.ca) site.
##### TODO: add a field to the servers table that keeps track of whether or not the server is a fedora instance.

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
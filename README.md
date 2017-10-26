# Islandora Jobs Stack Monitor

To gather and report health and job processing for relevant jobs servers.

This module should be installed on each of the gearman worker servers as well as the gearman master server and part of the code can also be installed on the Fedora server in order to send the health values back to the underlying database.

Since this module only works with gearman (and the islandora_jobs module), but **not all servers** in the stack have that module installed, the dependency on that module is not enforced.

### The uls-tuque.ini configuration file
This file stores cricital information that is needed by the cron job in order to run.  If any one of the values are wrong, the `host_server_health` table can not be updated.  The critical values are all in the `islandora_jobs_monitor` and `mysql` sections.  An obfuscated version of this file is below for reference.

```[islandora_jobs_monitor]
jobs_database = islandora_jobs_monitor
server_id = 4
server_monitor = http://dev.gamera.library.pitt.edu/
...

[mysql]
database = gearman_04
username = drupal
password = ****************
host = ***********
```

**Note:** *until a new field is added to the servers table to track whether or not the given server is a fedora server, any server that is a fedora server needs to have a server_id value greater than 100.*

Other servers could be configured to be included in this as well (with the connections opened to PUT data from the servers over port 80, or via direct mysql commands).

### Database Information
The new `islandora_jobs_monitor` database is needed to store the info and server health statistics for the various servers that make up the components of the stack.
#### `servers`
This stores the general info for each server.  The ip_address values below have been obfuscated.

server_id | hostname | ip_address | is_worker | name | master_command 
--- | --- | --- | ---:| --- | --- 
0|pa-staff-01|???.???.171.66|0|Gamera
1|pa-gmworker-01|???.???.171.75|1|gmworker-01
2|pa-gmworker-02|???.???.171.78|1|gmworker-02|clear_tmp
3|pa-gmworker-03|???.???.171.79|1|gmworker-03
4|pa-gmworker-04|???.???.171.80|1|gmworker-04
100|pa-fed-staff-01|???.???.230.34|0|Fedora

#### `host_server_health`
This table stores the health records for each server.  The CRON process will either populate this, or sent a http PUT to the [islandora_jobs_monitor] "server_monitor" value (see section on uls-tuque.ini above) server which will save the values.
#### `host_error_log`
#### `host_gearman_job`

## Maintainers/Sponsors
Current maintainers:

* [Brian Gillingham](https://github.com/bgilling)
* [ULS Development Team](https://github.com/ulsdevteam)

## Development

If you would like to contribute to this module, please check out [CONTRIBUTING.md](CONTRIBUTING.md). In addition, we have helpful [Documentation for Developers](https://github.com/Islandora/islandora/wiki#wiki-documentation-for-developers) info, as well as our [Developers](http://islandora.ca/developers) section on the [Islandora.ca](http://islandora.ca) site.
##### TODO: add a field to the servers table that keeps track of whether or not the server is a fedora instance.

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)

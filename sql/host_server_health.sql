CREATE TABLE `host_server_health` (
	`hostname` VARCHAR(50) NOT NULL,
	`seconds_since_hour` INT(5) UNSIGNED NOT NULL,
	`cpu_percentage` VARCHAR(20) NOT NULL,
	`memory_percentage` VARCHAR(20) NOT NULL,
	`error_log_errors_in_last_100` INT(5) UNSIGNED NOT NULL,
	`error_log_errors_in_last_1000` INT(5) UNSIGNED NOT NULL,
	PRIMARY KEY (`hostname`, `seconds_since_hour`)
)
COLLATE='latin1_swedish_ci'
ENGINE=InnoDB
;
CREATE TABLE `host_error_log` (
	`hostname` VARCHAR(50) NOT NULL,
	`error_log_last_1000_lines` TEXT NULL,
	PRIMARY KEY (`hostname`)
)
COMMENT='This will store the last 1000 lines of the apache error_log file for each host and is only updated as needed.  There will only be 1 record per host here.'
COLLATE='latin1_swedish_ci'
ENGINE=InnoDB
;

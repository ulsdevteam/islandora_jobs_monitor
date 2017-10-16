CREATE TABLE `host_gearman_job` (
	`hostname` VARCHAR(50) NOT NULL,
	`PID` VARCHAR(50) NOT NULL,
	`job_name` VARCHAR(100) NOT NULL,
	`started` TIMESTAMP NULL DEFAULT NULL,
	`completed` TIMESTAMP NULL DEFAULT NULL,
	PRIMARY KEY (`hostname`, `PID`, `job_name`)
)
COLLATE='latin1_swedish_ci'
ENGINE=InnoDB
;
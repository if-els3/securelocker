-- creating the database (you can configure the name as you like)
CREATE DATABASE IF NOT EXISTS rfid_system;

USE rfid_system;

-- table for log RFID
CREATE TABLE IF NOT EXISTS rfid_logs (
    id INT(11) NOT NULL AUTO_INCREMENT,
    uid VARCHAR(50) NOT NULL,
    status ENUM('ALLOWED', 'DENIED') NOT NULL,
    log_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Table for allowed_uids (you must add the UID manually by reading the card)
CREATE TABLE IF NOT EXISTS allowed_uids (
    id INT(11) NOT NULL AUTO_INCREMENT,
    uid VARCHAR(50) NOT NULL UNIQUE,
    PRIMARY KEY (id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Given allowed_uid (only for example)
INSERT INTO allowed_uids (uid) VALUES ('821E8B3F'), ('8962D1B9'), ('62DDB87C') ON DUPLICATE KEY UPDATE uid = uid;

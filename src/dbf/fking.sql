-- MySQL dump 10.13  Distrib 5.7.24, for Linux (x86_64)
--
-- Host: localhost    Database: fking
-- ------------------------------------------------------
-- Server version	5.7.24-0ubuntu0.18.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `test`
--

DROP TABLE IF EXISTS `test`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `test` (
  `msg` char(60) COLLATE utf8_bin NOT NULL,
  `user` char(20) COLLATE utf8_bin DEFAULT NULL,
  `tnum` int(10) DEFAULT NULL,
  PRIMARY KEY (`msg`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='test';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `test`
--

LOCK TABLES `test` WRITE;
/*!40000 ALTER TABLE `test` DISABLE KEYS */;
INSERT INTO `test` VALUES ('TEST MSG','bob fergus',999999999);
/*!40000 ALTER TABLE `test` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;


--
-- enabled field denotes if this lock is enabled
-- alldayssame field denotes if all days were set the same on the config screen
-- sun_enable, etc. denotes an active timelock for that day
-- sun_lock denotes lock time for that day
-- sun_unlock denotes unlock time for that day

DROP TABLE IF EXISTS `locks`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `locks` (
  `lockname` char(10) COLLATE utf8_bin NOT NULL,
  `sun_lock` char(5) COLLATE utf8_bin NOT NULL,
  `sun_unlock` char(5) COLLATE utf8_bin NOT NULL,
  `mon_lock` char(5) COLLATE utf8_bin NOT NULL,
  `mon_unlock` char(5) COLLATE utf8_bin NOT NULL,
  `tue_lock` char(5) COLLATE utf8_bin NOT NULL,
  `tue_unlock` char(5) COLLATE utf8_bin NOT NULL,
  `wed_lock` char(5) COLLATE utf8_bin NOT NULL,
  `wed_unlock` char(5) COLLATE utf8_bin NOT NULL,
  `thu_lock` char(5) COLLATE utf8_bin NOT NULL,
  `thu_unlock` char(5) COLLATE utf8_bin NOT NULL,
  `fri_lock` char(5) COLLATE utf8_bin NOT NULL,
  `fri_unlock` char(5) COLLATE utf8_bin NOT NULL,
  `sat_lock` char(5) COLLATE utf8_bin NOT NULL,
  `sat_unlock` char(5) COLLATE utf8_bin NOT NULL,
  `enabled` int(1) COLLATE utf8_bin DEFAULT 0,
  `sun_enable` int(1) COLLATE utf8_bin DEFAULT 1,
  `mon_enable` int(1) COLLATE utf8_bin DEFAULT 1,
  `tue_enable` int(1) COLLATE utf8_bin DEFAULT 1,
  `wed_enable` int(1) COLLATE utf8_bin DEFAULT 1,
  `thu_enable` int(1) COLLATE utf8_bin DEFAULT 1,
  `fri_enable` int(1) COLLATE utf8_bin DEFAULT 1,
  `sat_enable` int(1) COLLATE utf8_bin DEFAULT 1,
  `alldayssame` int(1) COLLATE utf8_bin DEFAULT 0,
  `id` int(5) COLLATE utf8_bin auto_increment,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='locks';
/*!40101 SET character_set_client = @saved_cs_client */;

LOCK TABLES `locks` WRITE;
/*!40000 ALTER TABLE `locks` DISABLE KEYS */;
INSERT INTO `locks` VALUES ('outterdoor','10:20','01:10','00:00','01:15','00:00','01:15','00:00','01:15','00:00','01:45','00:00','01:15','00:00','01:15','1','1','1','1','1','1','1','1','0','1');
INSERT INTO `locks` VALUES ('innerdoor','00:00','01:10','00:00','01:10','00:00','01:15','00:00','01:20','00:00','01:45','00:00','01:15','00:00','01:15','0','1','1','1','1','1','1','1','0','2');
INSERT INTO `locks` VALUES ('shutter','10:20','01:10','00:00','01:10','00:00','01:30','00:00','01:20','00:00','01:45','00:00','01:15','00:00','01:15','0','1','1','1','1','1','1','1','0','3');
INSERT INTO `locks` VALUES ('sidecar','00:00','01:10','00:00','01:10','00:00','01:30','00:00','01:20','00:00','01:45','00:00','01:15','00:00','01:15','0','1','1','1','1','1','1','1','0','4');
INSERT INTO `locks` VALUES ('base','10:20','01:10','00:00','01:10','00:00','01:30','00:00','01:20','00:00','01:01','00:00','01:15','00:00','01:15','0','1','1','1','1','1','1','1','0','5');
/*!40000 ALTER TABLE `locks` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;



DROP TABLE IF EXISTS `utd_denom`;
/*!40102 SET @saved_cs_client     = @@character_set_client */;
/*!40102 SET character_set_client = utf8 */;
CREATE TABLE `utd_denom` (
  `col` int(1) NOT NULL,
  `tube_name` varchar(20) NOT NULL,
  `tube_value` int(5) NOT NULL,
  `tube_count` int(2) DEFAULT 0,
  `modified_on` datetime NOT NULL DEFAULT NOW() ON UPDATE NOW(),
  `country_code` varchar(8) DEFAULT "USD",
  `id` int(5) COLLATE utf8_bin auto_increment,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='locks';
/*!40102 SET character_set_client = @saved_cs_client */;


LOCK TABLES `utd_denom` WRITE;
INSERT INTO `utd_denom` VALUES ('1','PENNIES','50','0',NOW(),'USD','1');
INSERT INTO `utd_denom` VALUES ('2','NICKELS','200','0',NOW(),'USD','2');
INSERT INTO `utd_denom` VALUES ('3','DIMES','500','0',NOW(),'USD','3');
INSERT INTO `utd_denom` VALUES ('4','QUARTERS','1000','0',NOW(),'USD','4');
INSERT INTO `utd_denom` VALUES ('5','ONES','1000','0',NOW(),'USD','5');
INSERT INTO `utd_denom` VALUES ('6','FIVES','2000','0',NOW(),'USD','6');
INSERT INTO `utd_denom` VALUES ('7','TENS','2000','0',NOW(),'USD','7');
INSERT INTO `utd_denom` VALUES ('8','TWENTIES','4000','0',NOW(),'USD','8');

UNLOCK TABLES;



DROP TABLE IF EXISTS `currency`;
/*!40102 SET @saved_cs_client     = @@character_set_client */;
/*!40102 SET character_set_client = utf8 */;
CREATE TABLE `currency` (
  `country_code` varchar(5)  NOT NULL,
  `type` varchar(5) NOT NULL,
  `value` int(4) NOT NULL,
  `symbol` varchar(10) COLLATE utf8_bin NOT NULL,
  `id` int(5) COLLATE utf8_bin auto_increment,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='locks';


/* USD - all values are listed in cents */
LOCK TABLES `currency` WRITE;
INSERT INTO `currency` VALUES('USD','NOTE','100','$','1');
INSERT INTO `currency` VALUES('USD','NOTE','500','$','2');
INSERT INTO `currency` VALUES('USD','NOTE','1000','$','3');
INSERT INTO `currency` VALUES('USD','NOTE','2000','$','4');
INSERT INTO `currency` VALUES('USD','NOTE','5000','$','5');
INSERT INTO `currency` VALUES('USD','NOTE','10000','$','6');

INSERT INTO `currency` VALUES('USD','COIN','1','c','7');
INSERT INTO `currency` VALUES('USD','COIN','5','c','8');
INSERT INTO `currency` VALUES('USD','COIN','10','c','9');
INSERT INTO `currency` VALUES('USD','COIN','25','c','10');


/* EURO - all values are listed in centimes
	written €50
	100 centimes = 1 euro
*/
INSERT INTO `currency` VALUES('EURO','NOTE','500','€','11');
INSERT INTO `currency` VALUES('EURO','NOTE','1000','€','12');
INSERT INTO `currency` VALUES('EURO','NOTE','2000','€','13');
INSERT INTO `currency` VALUES('EURO','NOTE','5000','€','14');
INSERT INTO `currency` VALUES('EURO','NOTE','10000','€','15');
INSERT INTO `currency` VALUES('EURO','NOTE','20000','€','16');
INSERT INTO `currency` VALUES('EURO','NOTE','50000','€','17');

INSERT INTO `currency` VALUES('EURO','COIN','1','c','18');
INSERT INTO `currency` VALUES('EURO','COIN','2','c','19');
INSERT INTO `currency` VALUES('EURO','COIN','5','c','20');
INSERT INTO `currency` VALUES('EURO','COIN','10','c','21');
INSERT INTO `currency` VALUES('EURO','COIN','20','c','22');
INSERT INTO `currency` VALUES('EURO','COIN','50','c','23');
INSERT INTO `currency` VALUES('EURO','COIN','100','€','24');
INSERT INTO `currency` VALUES('EURO','COIN','200','€','25');

/* PESO all values are listed in centavos 
	written $10
	100 centavos = 1 peso

*/

INSERT INTO `currency` VALUES('PESO','NOTE','2000','$','26');
INSERT INTO `currency` VALUES('PESO','NOTE','5000','$','27');
INSERT INTO `currency` VALUES('PESO','NOTE','10000','$','28');
INSERT INTO `currency` VALUES('PESO','NOTE','20000','$','29');
INSERT INTO `currency` VALUES('PESO','NOTE','50000','$','30');

INSERT INTO `currency` VALUES('PESO','COIN','100','$','31');
INSERT INTO `currency` VALUES('PESO','COIN','200','$','32');
INSERT INTO `currency` VALUES('PESO','COIN','500','$','33');
INSERT INTO `currency` VALUES('PESO','COIN','1000','$','34');
INSERT INTO `currency` VALUES('PESO','COIN','2000','$','35');
INSERT INTO `currency` VALUES('PESO','COIN','20','¢','36');
INSERT INTO `currency` VALUES('PESO','NOTE','50','¢','37');

/* SHEKEL all values are listed in agorot
	written ₪110 (altho, when written right to left, it appears at the end of the number)
	unicode 8362
	100 agorot = 1 shekel
*/


INSERT INTO `currency` VALUES('ILS','NOTE','2000','₪ ','38');

INSERT INTO `currency` VALUES('ILS','NOTE','5000','₪','39');
INSERT INTO `currency` VALUES('ILS','NOTE','10000','₪','40');
INSERT INTO `currency` VALUES('ILS','NOTE','20000','₪','41');

INSERT INTO `currency` VALUES('ILS','COIN','100','₪','42');
INSERT INTO `currency` VALUES('ILS','COIN','200','₪','43');
INSERT INTO `currency` VALUES('ILS','COIN','500','₪','44');
INSERT INTO `currency` VALUES('ILS','COIN','1000','₪','45');

INSERT INTO `currency` VALUES('ILS','COIN','10','agorot','46');
INSERT INTO `currency` VALUES('ILS','COIN','50','agorot','47');


UNLOCK TABLES;



DROP TABLE IF EXISTS `config`;
/*!40102 SET @saved_cs_client     = @@character_set_client */;
/*!40102 SET character_set_client = utf8 */;
CREATE TABLE `config` (
  `lang_id` int(5)  NOT NULL DEFAULT 1,
  `serial` varchar(30) NOT NULL,
  `custname` varchar(40),
  `safe_name` varchar(40)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='safe configuration';

LOCK TABLES `config` WRITE;
INSERT INTO `config` VALUES('1','GC2019055001','Bob Fergus','Bobs Safe');
UNLOCK TABLES;


DROP TABLE IF EXISTS `langs`;
/*!40102 SET @saved_cs_client     = @@character_set_client */;
/*!40102 SET character_set_client = utf8 */;
CREATE TABLE `langs` (
  `code` varchar(5)  NOT NULL,
  `name` varchar(40) NOT NULL,
  `active` tinyint NOT NULL DEFAULT 0,
  `id` int(5) COLLATE utf8_bin auto_increment,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='supported languages';


LOCK TABLES `langs` WRITE;
INSERT INTO `langs` VALUES('en','English',1,1);
INSERT INTO `langs` VALUES('es','Español',0,2);
INSERT INTO `langs` VALUES('fr','Français',0,3);
INSERT INTO `langs` VALUES('he','עברית',0,4);
UNLOCK TABLES;



DROP TABLE IF EXISTS `users`;
/*!40102 SET @saved_cs_client     = @@character_set_client */;
/*!40102 SET character_set_client = utf8 */;
CREATE TABLE `users` (
  `username` varchar(20)  NOT NULL,
  `password` varchar(40) NOT NULL,
  `fname` varchar(30) NOT NULL,
  `lname` varchar(30) NOT NULL,
  `status` tinyint DEFAULT -1,
  `user_level` int(5) NOT NULL DEFAULT 0,
  `dept` varchar(30),
  `created` datetime DEFAULT NOW(),
  `lastmodified` datetime DEFAULT NOW() ON UPDATE NOW(),
  `active` tinyint NOT NULL DEFAULT 0,
  `id` int(5) COLLATE utf8_bin auto_increment,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='users';




-- Dump completed on 2019-01-16 10:26:52

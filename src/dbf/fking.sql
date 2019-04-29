-- MySQL dump 10.13  Distrib 5.7.25, for Linux (x86_64)
--
-- Host: localhost    Database: fking
-- ------------------------------------------------------
-- Server version	5.7.25-0ubuntu0.18.04.2

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
-- Table structure for table `config`
--

DROP TABLE IF EXISTS `config`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `config` (
  `lang_id` int(5) NOT NULL DEFAULT '1',
  `serial` varchar(30) COLLATE utf8_bin NOT NULL,
  `custname` varchar(40) COLLATE utf8_bin DEFAULT NULL,
  `safe_name` varchar(40) COLLATE utf8_bin DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='safe configuration';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `config`
--

LOCK TABLES `config` WRITE;
/*!40000 ALTER TABLE `config` DISABLE KEYS */;
INSERT INTO `config` VALUES (1,'GC2019055001','Bob Fergus','Bobs Safe');
/*!40000 ALTER TABLE `config` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `currency`
--

DROP TABLE IF EXISTS `currency`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `currency` (
  `country_code` varchar(5) COLLATE utf8_bin NOT NULL,
  `type` varchar(5) COLLATE utf8_bin NOT NULL,
  `value` int(4) NOT NULL,
  `symbol` varchar(10) COLLATE utf8_bin NOT NULL,
  `id` int(5) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=48 DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='locks';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `currency`
--

LOCK TABLES `currency` WRITE;
/*!40000 ALTER TABLE `currency` DISABLE KEYS */;
INSERT INTO `currency` VALUES ('USD','NOTE',100,'$',1),('USD','NOTE',500,'$',2),('USD','NOTE',1000,'$',3),('USD','NOTE',2000,'$',4),('USD','NOTE',5000,'$',5),('USD','NOTE',10000,'$',6),('USD','COIN',1,'c',7),('USD','COIN',5,'c',8),('USD','COIN',10,'c',9),('USD','COIN',25,'c',10),('EURO','NOTE',500,'€',11),('EURO','NOTE',1000,'€',12),('EURO','NOTE',2000,'€',13),('EURO','NOTE',5000,'€',14),('EURO','NOTE',10000,'€',15),('EURO','NOTE',20000,'€',16),('EURO','NOTE',50000,'€',17),('EURO','COIN',1,'c',18),('EURO','COIN',2,'c',19),('EURO','COIN',5,'c',20),('EURO','COIN',10,'c',21),('EURO','COIN',20,'c',22),('EURO','COIN',50,'c',23),('EURO','COIN',100,'€',24),('EURO','COIN',200,'€',25),('PESO','NOTE',2000,'$',26),('PESO','NOTE',5000,'$',27),('PESO','NOTE',10000,'$',28),('PESO','NOTE',20000,'$',29),('PESO','NOTE',50000,'$',30),('PESO','COIN',100,'$',31),('PESO','COIN',200,'$',32),('PESO','COIN',500,'$',33),('PESO','COIN',1000,'$',34),('PESO','COIN',2000,'$',35),('PESO','COIN',20,'¢',36),('PESO','NOTE',50,'¢',37),('ILS','NOTE',2000,'₪ ',38),('ILS','NOTE',5000,'₪',39),('ILS','NOTE',10000,'₪',40),('ILS','NOTE',20000,'₪',41),('ILS','COIN',100,'₪',42),('ILS','COIN',200,'₪',43),('ILS','COIN',500,'₪',44),('ILS','COIN',1000,'₪',45),('ILS','COIN',10,'agorot',46),('ILS','COIN',50,'agorot',47);
/*!40000 ALTER TABLE `currency` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `depts`
--

DROP TABLE IF EXISTS `depts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `depts` (
  `department` varchar(50) COLLATE utf8_bin NOT NULL,
  `created` datetime DEFAULT CURRENT_TIMESTAMP,
  `lastmodified` datetime DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `id` int(5) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='depts';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `depts`
--

LOCK TABLES `depts` WRITE;
/*!40000 ALTER TABLE `depts` DISABLE KEYS */;
INSERT INTO `depts` VALUES ('DEFAULT','2019-04-23 14:53:51','2019-04-23 14:53:51',1);
/*!40000 ALTER TABLE `depts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `langs`
--

DROP TABLE IF EXISTS `langs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `langs` (
  `code` varchar(5) COLLATE utf8_bin NOT NULL,
  `name` varchar(40) COLLATE utf8_bin NOT NULL,
  `active` tinyint(4) NOT NULL DEFAULT '0',
  `id` int(5) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='supported languages';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `langs`
--

LOCK TABLES `langs` WRITE;
/*!40000 ALTER TABLE `langs` DISABLE KEYS */;
INSERT INTO `langs` VALUES ('en','English',1,1),('es','Español',1,2),('fr','Français',1,3),('he','עברית',0,4);
/*!40000 ALTER TABLE `langs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `locks`
--

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
  `enabled` int(1) DEFAULT '0',
  `sun_enable` int(1) DEFAULT '1',
  `mon_enable` int(1) DEFAULT '1',
  `tue_enable` int(1) DEFAULT '1',
  `wed_enable` int(1) DEFAULT '1',
  `thu_enable` int(1) DEFAULT '1',
  `fri_enable` int(1) DEFAULT '1',
  `sat_enable` int(1) DEFAULT '1',
  `alldayssame` int(1) DEFAULT '0',
  `id` int(5) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='locks';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `locks`
--

LOCK TABLES `locks` WRITE;
/*!40000 ALTER TABLE `locks` DISABLE KEYS */;
INSERT INTO `locks` VALUES ('outterdoor','10:20','01:10','00:00','01:15','00:00','01:15','00:00','01:15','00:00','01:45','00:00','01:15','00:00','01:15',1,1,1,1,1,1,1,1,0,1),('innerdoor','00:00','01:10','00:00','01:10','00:00','01:15','00:00','01:20','00:00','01:45','00:00','01:15','00:00','01:15',0,1,1,1,1,1,1,1,0,2),('shutter','10:20','01:10','00:00','01:10','00:00','01:30','00:00','01:20','00:00','01:45','00:00','01:15','00:00','01:15',0,1,1,1,1,1,1,1,0,3),('sidecar','00:00','01:10','00:00','01:10','00:00','01:30','00:00','01:20','00:00','01:45','00:00','01:15','00:00','01:15',0,1,1,1,1,1,1,1,0,4),('base','10:20','01:10','00:00','01:10','00:00','01:30','00:00','01:20','00:00','01:01','00:00','01:15','00:00','01:15',0,1,1,1,1,1,1,1,0,5);
/*!40000 ALTER TABLE `locks` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `perms`
--

DROP TABLE IF EXISTS `perms`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `perms` (
  `user_level` int(5) NOT NULL DEFAULT '-1',
  `cr` tinyint(4) DEFAULT '0',
  `add_users` tinyint DEFAULT '0',
  `edit_users` tinyint DEFAULT '0',
  `deposits` tinyint DEFAULT '0',
  `reports` tinyint DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='permissions';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `perms`
--

LOCK TABLES `perms` WRITE;
/*!40000 ALTER TABLE `perms` DISABLE KEYS */;
INSERT INTO `perms` VALUES (99,1,1,1,1,1);
/*!40000 ALTER TABLE `perms` ENABLE KEYS */;
UNLOCK TABLES;

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

--
-- Table structure for table `user_levels`
--

DROP TABLE IF EXISTS `user_levels`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `user_levels` (
  `name` varchar(20) COLLATE utf8_bin NOT NULL,
  `level` int(5) NOT NULL DEFAULT '-1'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='user levels';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user_levels`
--

LOCK TABLES `user_levels` WRITE;
/*!40000 ALTER TABLE `user_levels` DISABLE KEYS */;
INSERT INTO `user_levels` VALUES ('ADMIN',99),('MGR',50),('CASHIER',10),('GOD',999);
/*!40000 ALTER TABLE `user_levels` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `users` (
  `username` varchar(20) COLLATE utf8_bin NOT NULL,
  `password` varchar(80) COLLATE utf8_bin NOT NULL,
  `lang` varchar(2) COLLATE utf8_bin NOT NULL DEFAULT 'en',
  `fname` varchar(30) COLLATE utf8_bin NOT NULL,
  `lname` varchar(30) COLLATE utf8_bin NOT NULL,
  `user_level` int(5) NOT NULL DEFAULT '0',
  `dept` varchar(30) COLLATE utf8_bin DEFAULT NULL,
  `created` datetime DEFAULT CURRENT_TIMESTAMP,
  `lastmodified` datetime DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `active` tinyint(4) NOT NULL DEFAULT '0',
  `id` int(5) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='users';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `users`
--

LOCK TABLES `users` WRITE;
/*!40000 ALTER TABLE `users` DISABLE KEYS */;
INSERT INTO `users` VALUES ('0000','*97E7471D816A37E38510728AEA47440F9C6E2585','en','Gary','Conway',999,'GOD','2019-04-23 14:53:49','2019-04-23 14:53:49',1,1);
/*!40000 ALTER TABLE `users` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `utd_denom`
--

DROP TABLE IF EXISTS `utd_denom`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `utd_denom` (
  `col` int(1) NOT NULL,
  `tube_name` varchar(20) COLLATE utf8_bin NOT NULL,
  `tube_value` int(5) NOT NULL,
  `tube_count` int(2) DEFAULT '0',
  `modified_on` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `country_code` varchar(8) COLLATE utf8_bin DEFAULT 'USD',
  `id` int(5) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='locks';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `utd_denom`
--

LOCK TABLES `utd_denom` WRITE;
/*!40000 ALTER TABLE `utd_denom` DISABLE KEYS */;
INSERT INTO `utd_denom` VALUES (1,'PENNIES',50,0,'2019-04-23 14:53:43','USD',1),(2,'NICKELS',200,0,'2019-04-23 14:53:43','USD',2),(3,'DIMES',500,0,'2019-04-23 14:53:43','USD',3),(4,'QUARTERS',1000,0,'2019-04-23 14:53:43','USD',4),(5,'ONES',1000,0,'2019-04-23 14:53:43','USD',5),(6,'FIVES',2000,0,'2019-04-23 14:53:43','USD',6),(7,'TENS',2000,0,'2019-04-23 14:53:43','USD',7),(8,'TWENTIES',4000,0,'2019-04-23 14:53:43','USD',8);
/*!40000 ALTER TABLE `utd_denom` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

DROP TABLE IF EXISTS `inventory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `inventory` (
  `device_id` int(5),
  `field_1` float(12,2) DEFAULT 0,
  `field_2` float(12,2) DEFAULT 0,
  `field_3` float(12,2) DEFAULT 0,
  `field_4` float(12,2) DEFAULT 0,
  `field_5` float(12,2) DEFAULT 0,
  `field_6` float(12,2) DEFAULT 0,
  `id` int(5) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='inventory';
/*!40101 SET character_set_client = @saved_cs_client */;

DROP TABLE IF EXISTS `inv_map`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `inv_map` (
  `field` int(5),
  `denom` int(5),
  `units` varchar(10),
  `id` int(5) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8 COLLATE=utf8_bin COMMENT='inv_map';
/*!40101 SET character_set_client = @saved_cs_client */;



-- Dump completed on 2019-04-24  9:49:52

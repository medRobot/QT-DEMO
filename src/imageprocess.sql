/*
Navicat MySQL Data Transfer

Source Server         : localhost
Source Server Version : 50624
Source Host           : localhost:3306
Source Database       : imageprocess

Target Server Type    : MYSQL
Target Server Version : 50624
File Encoding         : 65001

Date: 2015-05-19 22:51:33
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `patient`
-- ----------------------------
DROP TABLE IF EXISTS `patient`;
CREATE TABLE `patient` (
  `id` int(5) DEFAULT NULL,
  `patientID` varchar(15) DEFAULT NULL,
  `name` varchar(15) DEFAULT NULL,
  `sex` varchar(15) DEFAULT NULL,
  `age` varchar(15) DEFAULT NULL,
  `modality` varchar(15) DEFAULT NULL,
  `time` varchar(50) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of patient
-- ----------------------------

-- phpMyAdmin SQL Dump
-- version 3.5.2.2
-- http://www.phpmyadmin.net
--
-- Host: 127.0.0.1
-- Generation Time: Dec 13, 2012 at 06:15 AM
-- Server version: 5.5.27
-- PHP Version: 5.4.7

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `univgrading`
--

-- --------------------------------------------------------

--
-- Table structure for table `course`
--

CREATE TABLE IF NOT EXISTS `course` (
  `courseno` varchar(6) NOT NULL,
  `cname` varchar(70) NOT NULL,
  `level` enum('5','6') NOT NULL,
  `dcode` varchar(10) NOT NULL,
  `dname` varchar(30) NOT NULL,
  PRIMARY KEY (`courseno`),
  KEY `dcode` (`dcode`),
  KEY `dname` (`dname`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `course`
--

INSERT INTO `course` (`courseno`, `cname`, `level`, `dcode`, `dname`) VALUES
('6303', 'OOAD', '6', '1233', 'CS'),
('6304', 'Computer Architecture', '6', '1234', 'CS'),
('6359', 'Database Design', '6', '1234', 'CS');

-- --------------------------------------------------------

--
-- Table structure for table `department`
--

CREATE TABLE IF NOT EXISTS `department` (
  `dname` varchar(30) NOT NULL,
  `dcode` varchar(10) NOT NULL,
  PRIMARY KEY (`dname`),
  UNIQUE KEY `dcode` (`dcode`),
  UNIQUE KEY `dname` (`dname`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `department`
--

INSERT INTO `department` (`dname`, `dcode`) VALUES
('CE', '1233'),
('CS', '1234');

-- --------------------------------------------------------

--
-- Table structure for table `grade`
--

CREATE TABLE IF NOT EXISTS `grade` (
  `snumber` varchar(32) NOT NULL,
  `courseno` varchar(6) NOT NULL,
  `secno` varchar(4) NOT NULL,
  `grade` enum('A','B','C','D','F') NOT NULL,
  `instructorid` varchar(32) NOT NULL,
  PRIMARY KEY (`snumber`,`courseno`,`secno`,`grade`),
  KEY `snumber` (`snumber`),
  KEY `courseno` (`courseno`),
  KEY `secno` (`secno`),
  KEY `grade` (`grade`),
  KEY `instructorid` (`instructorid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `grade`
--

INSERT INTO `grade` (`snumber`, `courseno`, `secno`, `grade`, `instructorid`) VALUES
('abhijit', '6303', '501', 'A', 'teacher1'),
('abhijit', '6304', '101', 'D', 'teacher1'),
('abhijit1', '6304', '101', 'C', 'teacher1'),
('snumber2', '6304', '101', 'B', 'teacher1'),
('snumber3', '6304', '101', '', 'teacher1'),
('snumber4', '6304', '101', '', 'teacher1'),
('abhijit', '6359', '101', 'A', 'teacher2');

-- --------------------------------------------------------

--
-- Table structure for table `instructor`
--

CREATE TABLE IF NOT EXISTS `instructor` (
  `fname` varchar(30) NOT NULL,
  `lname` varchar(30) NOT NULL,
  `instructorid` varchar(32) NOT NULL,
  `sex` enum('male','female') NOT NULL,
  `courseno` varchar(6) NOT NULL,
  `secno` varchar(4) NOT NULL,
  `loginid` varchar(14) NOT NULL,
  PRIMARY KEY (`instructorid`,`courseno`,`secno`),
  KEY `instructorid` (`instructorid`),
  KEY `courseno` (`courseno`),
  KEY `secno` (`secno`),
  KEY `loginid` (`loginid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `instructor`
--

INSERT INTO `instructor` (`fname`, `lname`, `instructorid`, `sex`, `courseno`, `secno`, `loginid`) VALUES
('Yiorgos', 'Makris', 'teacher1', 'male', '6304', '101', 'teacher1'),
('Yiorgos', 'Makris', 'teacher1', 'male', '6304', '501', 'teacher1'),
('Balaji', 'Raghavachari', 'teacher2', 'male', '6359', '101', 'teacher2'),
('Balaji', 'Raghavachari', 'teacher2', 'male', '6359', '501', 'teacher2');

-- --------------------------------------------------------

--
-- Table structure for table `logincredentials`
--

CREATE TABLE IF NOT EXISTS `logincredentials` (
  `loginid` varchar(14) NOT NULL,
  `password` varchar(32) NOT NULL,
  `acctPrivileges` enum('s','i','a') NOT NULL,
  PRIMARY KEY (`loginid`),
  KEY `loginid` (`loginid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `logincredentials`
--

INSERT INTO `logincredentials` (`loginid`, `password`, `acctPrivileges`) VALUES
('abhijit', '5f4dcc3b5aa765d61d8327deb882cf99', 's'),
('abhijit1', '5f4dcc3b5aa765d61d8327deb882cf99', 's'),
('Bryan', '5f4dcc3b5aa765d61d8327deb882cf99', 's'),
('Bull', '5f4dcc3b5aa765d61d8327deb882cf99', 's'),
('Mark', '5f4dcc3b5aa765d61d8327deb882cf99', 's'),
('Snarf', '5f4dcc3b5aa765d61d8327deb882cf99', 's'),
('Stuart', '5f4dcc3b5aa765d61d8327deb882cf99', 's'),
('teacher1', '5f4dcc3b5aa765d61d8327deb882cf99', 'i'),
('teacher2', '5f4dcc3b5aa765d61d8327deb882cf99', 'i');

-- --------------------------------------------------------

--
-- Table structure for table `section`
--

CREATE TABLE IF NOT EXISTS `section` (
  `secno` varchar(4) NOT NULL,
  `courseno` varchar(6) NOT NULL,
  PRIMARY KEY (`secno`,`courseno`),
  KEY `secno` (`secno`),
  KEY `courseno` (`courseno`),
  KEY `courseno_2` (`courseno`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `section`
--

INSERT INTO `section` (`secno`, `courseno`) VALUES
('101', '6303'),
('101', '6304'),
('101', '6359'),
('501', '6303'),
('501', '6304'),
('501', '6359');

-- --------------------------------------------------------

--
-- Table structure for table `student`
--

CREATE TABLE IF NOT EXISTS `student` (
  `fname` varchar(32) NOT NULL,
  `lname` varchar(32) NOT NULL,
  `snumber` varchar(32) NOT NULL,
  `sex` enum('male','female') NOT NULL,
  `DOB` date NOT NULL,
  `degree` enum('MS','BS') NOT NULL,
  `address` varchar(255) NOT NULL,
  `dname` varchar(30) NOT NULL,
  `dcode` varchar(10) NOT NULL,
  `courseno` varchar(6) NOT NULL,
  `secno` varchar(4) NOT NULL,
  `loginid` varchar(14) NOT NULL,
  PRIMARY KEY (`snumber`,`courseno`,`secno`),
  KEY `loginid` (`loginid`),
  KEY `secno` (`secno`),
  KEY `courseno` (`courseno`),
  KEY `dcode` (`dcode`),
  KEY `dname` (`dname`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `student`
--

INSERT INTO `student` (`fname`, `lname`, `snumber`, `sex`, `DOB`, `degree`, `address`, `dname`, `dcode`, `courseno`, `secno`, `loginid`) VALUES
('Abhijit', 'Rao', 'abhijit', 'male', '2012-12-02', 'MS', 'asd', 'CS', '1234', '6303', '501', 'abhijit'),
('Abhijit', 'Rao', 'abhijit', 'male', '2012-12-02', 'MS', 'asd', 'CS', '1234', '6304', '101', 'abhijit'),
('Abhijit', 'Rao', 'abhijit', 'male', '2012-12-02', 'MS', 'asd', 'CS', '1234', '6359', '101', 'abhijit'),
('Snarf', 'Studies', 'abhijit1', 'male', '2012-12-02', 'MS', 'asd', 'CS', '1234', '6304', '101', 'Snarf'),
('Bryan', 'Studies', 'abhijit1', 'male', '2012-12-02', 'MS', 'asd', 'CS', '1234', '6359', '101', 'Bryan'),
('Stuart', 'Rao', 'snumber2', 'male', '2012-12-02', 'MS', 'asd', 'CS', '1234', '6304', '101', 'Stuart'),
('Mark', 'Rao', 'snumber3', 'male', '2012-12-02', 'MS', 'asd', 'CS', '1234', '6304', '101', 'Mark'),
('Bull', 'Rao', 'snumber4', 'male', '2012-12-02', 'MS', 'asd', 'CS', '1234', '6304', '101', 'Bull');

--
-- Constraints for dumped tables
--

--
-- Constraints for table `course`
--
ALTER TABLE `course`
  ADD CONSTRAINT `course_ibfk_1` FOREIGN KEY (`dcode`) REFERENCES `department` (`dcode`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `course_ibfk_2` FOREIGN KEY (`dname`) REFERENCES `department` (`dname`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `grade`
--
ALTER TABLE `grade`
  ADD CONSTRAINT `grade_ibfk_1` FOREIGN KEY (`courseno`) REFERENCES `course` (`courseno`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `grade_ibfk_2` FOREIGN KEY (`secno`) REFERENCES `section` (`secno`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `grade_ibfk_4` FOREIGN KEY (`snumber`) REFERENCES `student` (`snumber`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `grade_ibfk_5` FOREIGN KEY (`instructorid`) REFERENCES `instructor` (`instructorid`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `instructor`
--
ALTER TABLE `instructor`
  ADD CONSTRAINT `instructor_ibfk_1` FOREIGN KEY (`courseno`) REFERENCES `course` (`courseno`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `instructor_ibfk_2` FOREIGN KEY (`secno`) REFERENCES `section` (`secno`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `instructor_ibfk_3` FOREIGN KEY (`loginid`) REFERENCES `logincredentials` (`loginid`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `section`
--
ALTER TABLE `section`
  ADD CONSTRAINT `section_ibfk_1` FOREIGN KEY (`courseno`) REFERENCES `course` (`courseno`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `student`
--
ALTER TABLE `student`
  ADD CONSTRAINT `student_ibfk_1` FOREIGN KEY (`dname`) REFERENCES `department` (`dname`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `student_ibfk_2` FOREIGN KEY (`courseno`) REFERENCES `course` (`courseno`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `student_ibfk_3` FOREIGN KEY (`secno`) REFERENCES `section` (`secno`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `student_ibfk_4` FOREIGN KEY (`loginid`) REFERENCES `logincredentials` (`loginid`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `student_ibfk_5` FOREIGN KEY (`dcode`) REFERENCES `department` (`dcode`) ON DELETE CASCADE ON UPDATE CASCADE;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;

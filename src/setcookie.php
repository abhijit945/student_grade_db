<?php 
session_start();
error_reporting (0);
if (isset($_COOKIE['currentID'])) {

	$loginid = $_COOKIE['currentID'];
	$acctPrivileges = $_COOKIE['accPriv'];
	$fname = $_COOKIE['fname'];
}
$hour = time() + 3600;
setcookie(courseno, $_GET['courseno'], $hour,'/');
setcookie(secno, $_GET['secno'], $hour,'/');
setcookie(snumber, $_GET['snumber'], $hour,'/');
setcookie(fname1, $_GET['fname1'], $hour,'/');
setcookie(lname, $_GET['lname'], $hour,'/');
Header( "Location: EditGrade.php" );
?>
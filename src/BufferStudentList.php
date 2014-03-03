
<?php
session_start();
error_reporting (0);
if (isset($_COOKIE['currentID'])) {

	$loginid = $_COOKIE['currentID'];
	$acctPrivileges = $_COOKIE['accPriv'];
	$fname = $_COOKIE['fname'];

}
require_once "DBConnect.php";
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
<title>Login: UnivGrading</title>
<meta http-equiv="content-type" content="text/html; charset=utf-8" />

<!-- stylesheets -->
<link rel="stylesheet" href="css/style.css" type="text/css"
	media="screen" />
<link rel="stylesheet" href="css/slide.css" type="text/css"
	media="screen" />

<script src="js/jquery-1.3.2.min.js" type="text/javascript"></script>
<!-- Sliding effect -->
<script src="js/slide.js" type="text/javascript"></script>

</head>
<?php 

$in= $_GET['txt'];
$courseno= $_GET['courseno'];
$secno = $_GET['secno'];
$snumber = "";
$t="";

if(strlen($in)> 0 and strlen($in) <20 ){
	$t=mysql_query("select * from student where (fname like '$in%' or lname like '$in%') and courseno = '$courseno' and secno = '$secno'");
}
else{
	$t=mysql_query("select * from student where courseno = '$courseno' and secno = '$secno'");
}

echo "<div id = 'container'>";
echo "<table border = '1' align = 'center'>
<tr>
<th> First Name </th>
<th> Last Name </th>
<th> Student ID </th>
<th> View or Edit Grade </th>
</tr>";

while($nt = mysql_fetch_array($t)){
	$tempSNumber = $nt['snumber'];
	$tempfname = $nt['fname'];
	$templname = $nt['lname'];
	echo "<tr>";
	echo "<td>" . $nt['fname'] . "</td>";
	echo "<td>" . $nt['lname'] . "</td>";
	echo "<td>" . $nt['snumber'] . "</td>";
	echo "<td> <a href='setcookie.php?courseno=$courseno&secno=$secno&snumber=$tempSNumber&fname1=$tempfname&lname=$templname'>Click Here</a> </td>";
	echo "</tr>";
}
echo "</table>";
?>
</html>
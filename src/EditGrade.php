<?php
session_start();
error_reporting (0);
if (isset($_COOKIE['currentID'])) {

	$loginid = $_COOKIE['currentID'];
	$acctPrivileges = $_COOKIE['accPriv'];
	$fname = $_COOKIE['fname'];
}

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
<title>Edit Grades: UnivGrading</title>
<meta http-equiv="content-type" content="text/html; charset=utf-8" />

<!-- stylesheets -->
<link rel="stylesheet" href="css/style.css" type="text/css"
	media="screen" />
<link rel="stylesheet" href="css/slide.css" type="text/css"
	media="screen" />

<script src="js/jquery-1.3.2.min.js" type="text/javascript"></script>

</head>
<body>

<div id="toppanel">
		<div class="tab">
			<ul class="login">
				<li class="left">&nbsp;</li>
				<li>Hello <?php echo $fname?>!</li>
				<li class="sep">|</li>
				<li id="logout"><a id="logout" class="logout" href="logout.php">Log
						out</a>
				</li>
				<li class="right">&nbsp;</li>
			</ul>
		</div>

	</div>

<?php 
require_once "DBConnect.php";
$cno = $_COOKIE['courseno'];
$sno = $_COOKIE['secno'];
$stuno = $_COOKIE['snumber'];
$fna= $_COOKIE['fname1'];
$lna= $_COOKIE['lname'];
$flag = 1;
if($_POST['submit']){
	
	$gradeValue = $_POST['dropdown'];
	$t3 = mysql_query("update grade set grade = '$gradeValue' where courseno = '$cno' and secno = '$sno' and snumber = '$stuno'");
}
	$t1 = mysql_query("select * from student where courseno = '$cno' and secno = '$sno' and snumber='$stuno'");
	$t = mysql_query("select grade from grade where courseno = '$cno' and secno = '$sno' and snumber='$stuno'");


echo "<div id = 'container'>";
echo "<table border = '1' align = 'center'>
<tr>
<th> First Name </th>
<th> Last Name </th>
<th> Student ID </th>
<th> Grade </th>
<th> Edit Grade </th>
</tr>";

while($nt1 = mysql_fetch_array($t1)){
	$nt = mysql_fetch_array($t);
	echo "<tr>";
	echo "<td>" . $fna . "</td>";
	echo "<td>" . $lna . "</td>";
	echo "<td>" . $stuno . "</td>";
	echo "<td>" . $nt['grade'] . "</td>";
	echo "<td><form action='EditGrade.php' method='post' enctype='multipart/form-data'>
	<select name='dropdown'>
	<option value='default'>Choose Grade</option>
	<option value='A'>A</option>
	<option value='B'>B</option>
	<option value='C'>C</option>
	<option value='D'>D</option>
	<option value='F'>F</option>
	</select></td>";
	echo "<input type='hidden' name='flagVal' value='".$flag."'>";
	echo "<td><input type='submit' name='submit' value='Submit'></form></td>";
	echo "</tr>";
}

echo "</table>";
echo "<form class='clearfix' action='CourseList.php' method='post'
						enctype='multipart/form-data' name='GoBackForm'>";
echo "<input type='submit' name='submit' value='GoBack' class='bt_login'/> </form></div>";
echo "</div>";


?>
</body>
</html>
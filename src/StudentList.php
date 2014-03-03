<?php error_reporting (0); ?>
<?php
session_start();
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
<title>Choose Courses: UnivGrading</title>
<meta http-equiv="content-type" content="text/html; charset=utf-8" />

<!-- stylesheets -->
<link rel="stylesheet" href="css/style.css" type="text/css"
	media="screen" />
<link rel="stylesheet" href="css/slide.css" type="text/css"
	media="screen" />

<script src="js/jquery-1.3.2.min.js" type="text/javascript"></script>
<script type="text/javascript">

function ajaxFunction(str)
{
var httpxml;
try
{
// Firefox, Opera 8.0+, Safari
httpxml=new XMLHttpRequest();
}
catch (e)
{
try
{
httpxml=new ActiveXObject("Msxml2.XMLHTTP");
}
catch (e)
{
try
{
httpxml=new ActiveXObject("Microsoft.XMLHTTP");
}
catch (e)
{
alert("Your browser does not support AJAX!");
return false;
}
}
}
function stateChanged() 
{
if(httpxml.readyState==4)
{
document.getElementById("displayDiv").innerHTML=httpxml.responseText;

}
}
var url="BufferStudentList.php";
url=url+"?txt="+str+"&courseno="+'<?php echo $_GET['courseno'] ?>'+"&secno="+'<?php echo $_GET['secno']?>';
url=url+"&sid="+Math.random();
httpxml.onreadystatechange=stateChanged;
httpxml.open("GET",url,true);
httpxml.send(null);
}
</script>
</head>
<body onload="ajaxFunction('')">

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

$courseno=$_GET['courseno'];
$secno = $_GET['secno'];

$t2 = mysql_query("select * from course where courseno = '$courseno'");
$nt2=mysql_fetch_array($t2);

$cnameTemp = $nt2['cname'];


echo "<div id='container'>";
echo "<table border='0' align = 'center'>";
echo "<b>Course No: " . "$courseno" . " <br> </tr>". "<b>Section: " . "$secno" ."<br>";
echo "<b>Course Name: " . "$cnameTemp";	
echo "<tr><td><p align ='justify'>Please type the name of the student to search: </td>"."<td>
<form name='myForm' align = 'center'>
					<input type='text' onkeyup='ajaxFunction(this.value);' name='fname' />
					
				</form></td><tr>";
echo "</table>";
echo "</div>";	

echo "<div id='displayDiv'></div>"

?>

</body>
</html>
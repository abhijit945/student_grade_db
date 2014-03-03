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
	switch ($acctPrivileges){
		case 's':
			{
				$t=mysql_query("select * from student where loginid = '$loginid'");
				echo "<div id='content'>";
				echo "<p align = 'center'><br>Here are the Courses you have taken <b>" . $fname . "</b> <br></p>";
				echo "<table border='1' align = 'center'>
	
				<tr>
				<th> Course No and Section </th>
				<th> Course Name </th>
				<th>  Grades </th>
				</tr>";
				while($nt=mysql_fetch_array($t)){
					$snumber = $nt['snumber'];
					$temp = $nt['courseno'];
					$temp2 = $nt['secno'];
					$temp3 = $nt['dname'];
					
					$t2 = mysql_query("select * from course where courseno = '$temp' and dname = '$temp3'");
					$nt2=mysql_fetch_array($t2);
					
					$t3 = mysql_query("select * from grade where courseno = '$temp' and snumber = '$snumber' and secno = '$temp2'");
					$nt3=mysql_fetch_array($t3);
					echo "<tr>";
					echo "<td>" .$nt['dname'] . $nt3['courseno'] . "." .  $nt3['secno'] ."</td>";
					echo "<td>" . $nt2['cname'] . "</td>";
					echo "<td>" . $nt3['grade'] . "</td>";
					echo "</tr>";
				}
				echo "</table>";
				echo "</div>";
				break;
			}
	
			
		case 'i':
			{
				$t=mysql_query("select * from instructor where loginid = '$loginid'");
				echo "<div id='content'>";
				echo "<p align = 'center'><br>Here are the Courses you are teaching Professor <b>" . $fname . "</b> <br></p>";
				echo "<table border='1' align = 'center'>
				<tr>
				<th> Course No and Section </th>
				<th> Course Name </th>
				<th>  Student Information </th>
				</tr>";
				while($nt=mysql_fetch_array($t))
				{
					$inumber = $nt['instructorid'];
					$temp = $nt['courseno'];
					$temp2 = $nt['secno'];
					$t2 = mysql_query("select * from course where courseno = '$temp'");
					$nt2=mysql_fetch_array($t2);
					
					$t3 = mysql_query("select dname from course where courseno = '$temp'");
					$nt3 = mysql_fetch_array($t3);
					
					
					
					$hour = time() + 3600;
					if($nt['courseno'] != null && $nt['secno'] != null){
						setcookie(coursenum, $nt['courseno'], $hour,'/');
						setcookie(sectionnum, $nt['secno'], $hour,'/');
					}
					else
					{
						echo "<div id='content'>
							<p class='highlight'>
							<Strong>Contact University for Account details as our records does not have it yet!!!</Strong>
							</p>
							</div>
							<style type='text/css'>#toppanel{display:none;}
												   #container{display:none;}
												   </style>";
						
					}
					$movToStudent = '<a href="StudentList.php?courseno='.$nt['courseno'].'&secno='.$nt['secno'].'"> Details </a>';
					echo "<tr>";
					echo "<td>" .$nt3['dname']. $nt['courseno'] . "." .  $nt['secno'] ."</td>";
					echo "<td>" . $nt2['cname'] . "</td>";
					echo "<td>" . $movToStudent . "</td>";
					echo "</tr>";
				}
				
				echo "</table>";
				echo "</div>";
				break;
			}
	}
	?>
</body>
</html>

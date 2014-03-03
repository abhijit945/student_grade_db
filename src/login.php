<?php error_reporting (0); ?>
<?php
session_start();
if($_POST['loginid']){
	ini_set( "display_errors", 0);
	require_once "DBConnect.php";
	$loginid = $_POST['loginid'];
	$password = $_POST['password'];
	$loginid = str_replace(" ","",$loginid);
	$loginid = eregi_replace("[^A-Za-z0-9]", "", $loginid);
	$password = eregi_replace("[^A-Za-z0-9]", "", $password);
	$password = md5($password);
	$loginQuery = "select * from logincredentials where loginid = '$loginid' and password = '$password'";
	
	$auth_login = mysql_query($loginQuery);
	$isLoginValid  = mysql_num_rows($auth_login);
	
	if($isLoginValid > 0){
		$hour = time() + 3600;
		setcookie(currentID, $_POST['loginid'], $hour,'/');
		setcookie(currentKey, $_POST['password'], $hour,'/');
		$fetchRows = mysql_fetch_array($auth_login);
		$loginid = $fetchRows["loginid"];
		$_SESSION['loginid'] = $loginid;
		$acctPrivileges = $fetchRows["acctPrivileges"];
		$_SESSION['acctPrivileges'] = $acctPrivileges;
		setcookie(accPriv, $acctPrivileges, $hour,'/');
		switch ($acctPrivileges){
			case 's':
				$query = "Select * from student where loginid = '$loginid'";
				
				$fetchBased_login = mysql_query($query);
				$isAuthvalid  = mysql_num_rows($fetchBased_login);
				
				if($isAuthvalid > 0){
					$fetchAuthRows = mysql_fetch_array($fetchBased_login);
					$fname = $fetchAuthRows["fname"];
					
					setcookie(fname, $fname, $hour,'/');
					$_SESSION['fname'] = $fname;
					
				}
				break;

			case 'i':
				$query = "Select * from instructor where loginid = '$loginid'";
				$fetchBased_login = mysql_query($query);
				$isAuthvalid  = mysql_num_rows($fetchBased_login);
				if($isAuthvalid > 0){
					$fetchAuthRows = mysql_fetch_array($fetchBased_login);
					$fname = $fetchAuthRows["fname"];
					setcookie(fname, $fname, $hour,'/');
					$_SESSION['fname'] = $fname;
					//Header( "Location: CourseList.php" ); 
				}
				break;
		}
		Header( "Location: CourseList.php" ); 
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

}
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

<script type="text/javascript">
	function validate_form() 
	{ 
		valid = true; 
		if ( document.logForm.loginid.value == "" || document.logForm.password.value == "" ) { 
			alert ("Username or Password cannot be left blank"); 
			valid = false;
		}
		return valid;
	}
	</script>
</head>

<body>
	<!-- Panel -->
	<div id="toppanel">
		<div id="panel">
			<div class="content clearfix">
				<div class="left">
					<h1>Welcome to The UnivGrading</h1>
					<h2>Instructors/Students Please login to access your account</h2>
				</div>
				<div class="left">
					<!-- Login Form -->
					<form class="clearfix" action="login.php" method="post"
						enctype="multipart/form-data" name="logForm"
						onsubmit="return validate_form();">
						<h1>Member Login</h1>
						<label class="grey" for="loginid">Username:</label> <input
							class="field" type="text" name="loginid" id="loginid" value="" size="14" />
						<label class="grey" for="password">Password:</label> <input
							class="field" type="password" name="password" id="password" size="20" /> <label><input
							name="rememberme" id="rememberme" type="checkbox"
							checked="checked" value="forever" /> &nbsp;Remember me</label>
						<div class="clear"></div>
						<input type="submit" name="submit" value="Login" class="bt_login" />
						<a class="lost-pwd" href="#">Lost your password?</a>
					</form>
				</div>
				<!-- 	<div class="left right">			
				<form action="Register.php" method="post" enctype="multipart/form-data">
					<h1>Not a member yet? Sign Up!</h1>				
					<label class="grey" for="signup">Username:</label>
					<input class="field" type="text" name="signup" id="signup" value="" size="23" />
					<label class="grey" for="password">Password:</label>
					<input class="field" type="text" name="password" id="password" size="23" />
					<label class="grey" for="CPassword">Confirm Password:</label>
					<input class="field" type="text" name="cpassword" id="cpassword" size="23" />
					<input type="submit" name="submit" value="Register" class="bt_register" />
				</form>
			</div>  -->
			</div>
		</div>
		<!-- /login -->

		<!-- The tab on top -->
		<div class="tab">
			<ul class="login">
				<li class="left">&nbsp;</li>
				<li>Hello Guest!</li>
				<li class="sep">|</li>
				<li id="toggle"><a id="open" class="open" href="#">Log In</a> <a
					id="close" style="display: none;" class="close" href="#">Close
						Panel</a>
				</li>
				<li class="right">&nbsp;</li>
			</ul>
		</div>

	</div>

	<div id="container">
		<div id="content" style="padding-top: 100px;">
			<h1>The UnivGrading</h1>
			<h2>A website to view Transcripts and Store Grades for Instructors
				&amp; Students</h2>
			<p class="highlight">
				Click "<strong>Log In</strong>" on top
			</p>

		</div>
	</div>
</body>
</html>

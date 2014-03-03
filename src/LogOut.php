<?php error_reporting (0); ?>
<?php
session_start();
$expire = time() - 100;
setcookie('currentID',"",$expire,'/');
setcookie('currentKey',"",$expire,'/');
setcookie('accPriv',"",$expire,'/');
setcookie('fname',"",$expire,'/');
session_destroy();
Header( "Location: login.php" );
?>

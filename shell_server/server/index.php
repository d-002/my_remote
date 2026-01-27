<?php session_start(); ?>

<html>
<head>
<?php
if ($_SESSION["username"] === NULL) {
?>
    <meta http-equiv="refresh" content="0; url=/login">
<?php
}
?>
    <title>shell_server - Dashboard</title>
</head>

<body>
</body>
</html>

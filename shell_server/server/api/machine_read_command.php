<?php
ob_start();

require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-commandlist.php");

$db = getDB();
$error = readFirstUserCommand($db, $_REQUEST["user"], $_REQUEST["machine"]);

echo $error === "" ? "success" : "error: " . $error;

$size = ob_get_length();
header("Content-Length: " . $size);
ob_end_flush();
?>

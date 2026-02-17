<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-commandlist.php");

$db = getDB();

$error = clearShell($db, $_REQUEST["user"], $_REQUEST["machine"]);

echo $error === "" ? "success" : "error: " . $error;
?>

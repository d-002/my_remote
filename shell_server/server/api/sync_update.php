<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-version.php");

$db = getDB();

$error = syncUpdate($db, $_REQUEST["user"]);

echo $error === "" ? "success" : "error: " . $error;
?>

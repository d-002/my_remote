<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-login.php");

$db = getDB();

$error = deleteUser($db, $_REQUEST["username"], $_REQUEST["password"]);

echo $error === "" ? "success" : "error: " . $error;
?>

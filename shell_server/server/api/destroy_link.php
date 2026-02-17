<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-machines.php");

$db = getDB();

$error = removeLink($db, $_REQUEST["user"], $_REQUEST["machine"]);

echo $error === "" ? "success" : "error: " . $error;
?>

<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-machines.php");

$db = getDB();
$name = trim($_REQUEST["name"]);

if ($name === "") {
    $error = "Cannot have an empty machine name.";
}
else {
    $error = renameMachine($db, $_REQUEST["user"], $_REQUEST["machine"], $name);
}

echo $error === "" ? "success" : "error: " . $error;
?>

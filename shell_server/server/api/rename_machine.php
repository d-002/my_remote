<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-machines.php");

$db = getDB();
$name = trim($_REQUEST["name"]);

if ($name === "") {
    $error = "Cannot have an empty machine name.";
}
else if (strlen($name) > 32) {
    $error = "Machine name too long.";
}
else {
    $error = renameMachine($db, $_REQUEST["user"], $_REQUEST["machine"], $name);
}

echo $error === "" ? "success" : "error: " . $error;
?>

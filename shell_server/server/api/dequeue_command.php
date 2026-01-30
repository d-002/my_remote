<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-commandlist.php");

$db = getDB();

$error = "";
$st = getUserMachineCommands($db, $_REQUEST["user"], $_REQUEST["machine"]);

if ($st === NULL) {
    $error = "Could not get command.";
}

else {
    $error = deleteFirstCommand($db, $st);
}

echo $error === "" ? "success" : "error: " . $error;
?>

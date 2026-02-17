<?php
ob_start();

require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-machines.php");

$db = getDB();

$state = getMachineState($db, $_REQUEST["machine"]);

echo $state === NULL ? "error: Could not get machine state." : $state;

$size = ob_get_length();
header("Content-Length: " . $size);
ob_end_flush();
?>

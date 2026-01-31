<?php
ob_start();

require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-commandlist.php");

$db = getDB();
$link_id = getUserMachineLink($db, $_REQUEST["user"], $_REQUEST["machine"]);

$error = "";
if ($link_id === NULL) {
    $error = "Could not get user/machine link.";
}
else {
    $is_user = $_REQUEST["is_user"] === NULL ? 0 : 1;
    $error = enqueueCommand($db, $link_id, $is_user, $_REQUEST["content"]);
}

echo $error === "" ? "success" : "error: " . $error;

$size = ob_get_length();
ob_end_flush();
header("Content-Length: " . $size);
?>

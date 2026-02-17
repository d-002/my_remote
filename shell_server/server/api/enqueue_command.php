<?php
ob_start();

require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-commandlist.php");

$db = getDB();
$link_id = getUserMachineLink($db, $_REQUEST["user"], $_REQUEST["machine"]);
$status = $_REQUEST["status"];
if ($status === NULL) {
    $status = "normal";
}

$error = "";
if ($link_id === NULL) {
    $error = "Could not get user/machine link.";
}
else {
    $is_user = $_REQUEST["is_user"] === NULL ? 0 : 1;

    $content = file_get_contents("php://input");
    if ($content === false) {
        $error = "Could not read POST raw content";
    }

    $error = enqueueCommand($db, $link_id, $is_user, $status, $content);
}

echo $error === "" ? "success" : "error: " . $error;

$size = ob_get_length();
header("Content-Length: " . $size);
ob_end_flush();
?>

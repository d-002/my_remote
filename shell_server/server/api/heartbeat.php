<?php
ob_start();

require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-machines.php");
my_include("/utils/db-status.php");

$user_hash = $_REQUEST["user"];
$machine_hash = $_REQUEST["machine"];

$db = getDB();
$link_id = getUserMachineLink($db, $user_hash, $machine_hash);

$error = "";
$outData = "";
if ($link_id === NULL) {
    $error = "Could not get user/machine link.";
}
else {
    $inData = file_get_contents("php://input");
    if ($inData === false) {
        $error = "Could not read POST raw outData";
    }

    else {
        $lines = explode("\n", trim($inData));

        $status = $lines[0];
        $machine_version = $lines[1];

        $software = getSoftware($db, $user_hash);
        if ($software === NULL) {
            return "error: Could not fetch user software version";
        }
        $user_version = $software["version"];

        $error = updateMachineHeartbeat($db, $machine_hash);

        if ($error === "") {
            if (version_compare($machine_version, $user_version) < 0) {
                $outData = $user_version . "\n" . $software["binary"];
            }
        }
    }
}

if ($outData === "") {
    echo $error === "" ? "success" : "error: " . $error;
}
else {
    echo "update\n" . $user_version . "\n" . $outData;
}

$size = ob_get_length();
header("Content-Length: " . $size);
ob_end_flush();
?>

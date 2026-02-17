<?php
ob_start();

require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-commandlist.php");

// notice: if either hash is wrong, no commands will be displayed withouh an
// error.

$db = getDB();

$st = getUserMachineCommands($db, $_REQUEST["user"], $_REQUEST["machine"]);
if ($st === NULL) {
    echo "error: Could not list commands.";
}
else {
    while (true) {
        $current = $st->fetch();
        if ($current === false) {
            break;
        }

        $prefix = $current["is_user"] === 0 ? "machine" : "user";
        $prefix .= " ";
        $prefix .= $current["is_read"] === 0 ? "pending" : "read";
        $prefix .= " ";
        $prefix .= $current["status"];
        $prefix .= " ";
        $prefix .= $current["timestamp"];
        $prefix .= ": ";
        echo $prefix . $current["content"] . "\n";
    }

    $st->closeCursor();
}

$size = ob_get_length();
header("Content-Length: " . $size);
ob_end_flush();
?>

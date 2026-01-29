<?php
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

        echo $current["content"] . "\n";
    }

    $st->closeCursor();
}
?>

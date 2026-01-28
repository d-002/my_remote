<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");

// notice: if either hash is wrong, no commands will be displayed withouh an
// error.

function getRequest() {
    $user_hash = $_REQUEST["user"];
    $machine_hash = $_REQUEST["machine"];

    $db = getDB();

    try {
        $st = $db->prepare("
WITH target AS (
    SELECT links.id
    FROM links
    JOIN users
        ON links.user_id = users.id
    JOIN machines
        ON links.machine_id = machines.id
    WHERE users.hash = :user_hash
    AND machines.hash = :machine_hash
)
SELECT commands.content
FROM commands
JOIN target
    ON commands.link_id = target.id");

        $st->execute(["user_hash" => $user_hash, "machine_hash" => $machine_hash]);

        return $st;
    }
    catch (Exception $e) {
        return NULL;
    }
}

$st = getRequest();
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

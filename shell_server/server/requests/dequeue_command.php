<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");

function getError() {
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
SELECT commands.id
FROM commands
JOIN target
    ON commands.link_id = target.id
ORDER BY commands.id ASC
LIMIT 1");

        $st->execute(["user_hash" => $user_hash, "machine_hash" => $machine_hash]);
    }
    catch (Exception $e) {
        return "error: Could not get command.";
    }

    $command_id = $st->fetchColumn();
    $st->closeCursor();

    if ($command_id === NULL) {
        return "error: Could not get command.";
    }

    try {
        $st = $db->prepare("
DELETE FROM commands
WHERE commands.id = :id");
        $st->execute(["id" => $command_id]);
    }
    catch (Exception $e) {
        return "error: Could not delete command.";
    }

    return "success";
}

echo getError();
?>

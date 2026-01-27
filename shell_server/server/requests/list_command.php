<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");

$user_hash = $_REQUEST["user"];
$machine_hash = $_REQUEST["machine"];

$db = getDB();

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

while (true) {
    $current = $st->fetch();
    if ($current === false) {
        break;
    }

    echo $current["content"] . "\n";
}

$st->closeCursor();
?>

<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");

$user_hash = $_REQUEST["user"];
$machine_hash = $_REQUEST["machine"];
$content = $_REQUEST["content"];

$db = getDB();

$st = $db->prepare("
SELECT links.id
FROM links
JOIN users
    ON links.user_id = users.id
JOIN machines
    ON links.machine_id = machines.id
WHERE users.hash = :user_hash
AND machines.hash = :machine_hash");

$st->execute(["user_hash" => $user_hash, "machine_hash" => $machine_hash]);
$link_id = $st->fetchColumn();
$st->closeCursor();

// notice the !== here
if ($link_id !== false) {
    $st = $db->prepare("
INSERT INTO commands
(link_id, content)
VALUES
(:link_id, :content)");

    $st->execute(["link_id" => $link_id, "content" => $content]);
}
?>

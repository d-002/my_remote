<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");

function getError() {
    $user_hash = $_REQUEST["user"];
    $machine_hash = $_REQUEST["machine"];
    $content = $_REQUEST["content"];

    $db = getDB();

    try {
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
    }
    catch (Exception $e) {
        return "error: Could not get user/machine link.";
    }

    $link_id = $st->fetchColumn();
    $st->closeCursor();

    if ($link_id === NULL) {
        return "error: Could not get user/machine link.";
    }

    try {
        $st = $db->prepare("
INSERT INTO commands
    (link_id, content)
VALUES
    (:link_id, :content)");

        $st->execute(["link_id" => $link_id, "content" => $content]);
    }
    catch (Exception $e) {
        return "error: Could not add command.";
    }

    return "success";
}

echo getError();
?>

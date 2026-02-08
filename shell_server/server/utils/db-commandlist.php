<?php
// must be included
my_include("/utils/db-machines.php");

function enqueueCommand($db, $link_id, $is_user, $content) {
    try {
        $st = $db->prepare("
INSERT INTO commands
    (link_id, is_user, timestamp, content)
VALUES
    (:link_id, :is_user, :timestamp, :content)");

        $st->execute(["link_id" => $link_id,
                      "is_user" => $is_user,
                      "timestamp" => time(),
                      "content" => $content]);
    }
    catch (Exception $e) {
        return "Could not add command.";
    }

    return "";
}

function readFirstUserCommand($db, $user_hash, $machine_hash) {
    $link_id = getUserMachineLink($db, $user_hash, $machine_hash);

    if ($link_id === NULL) {
        return "Could not find link.";
    }

    try {
        $st = $db->prepare("
UPDATE commands
SET is_read = 1
WHERE commands.id = (
    SELECT commands.id
    FROM commands
    WHERE commands.link_id = :link_id
        AND commands.is_user = 1
        AND commands.is_read = 0
    ORDER BY commands.timestamp ASC
    LIMIT 1
)");
        $st->execute(["link_id" => $link_id]);
    }
    catch (Exception $e) {
        return "Could not read command.";
    }

    return "";
}

function clearShell($db, $user_hash, $machine_hash) {
    $link_id = getUserMachineLink($db, $user_hash, $machine_hash);

    if ($link_id === NULL) {
        return "Could not find link.";
    }

    try {
        $st = $db->prepare("
DELETE FROM commands
WHERE commands.link_id = :link_id
    AND is_user = 0
    OR is_read = 1");
        $st->execute(["link_id" => $link_id]);
    }
    catch (Exception $e) {
        return "Could not delete commands.";
    }

    return "";
}
?>

<?php
function getUserMachineLink($db, $user_hash, $machine_hash) {
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
        return "Could not get user/machine link.";
    }

    $link_id = $st->fetchColumn();
    $st->closeCursor();

    return $link_id === false ? NULL : $link_id;
}

function getUserMachineCommands($db, $user_hash, $machine_hash) {
    $link_id = getUserMachineLink($db, $user_hash, $machine_hash);

    if ($link_id === NULL) {
        return NULL;
    }

    try {
        $st = $db->prepare("
SELECT commands.*
FROM commands
WHERE commands.link_id = :link_id
ORDER BY commands.id ASC");

        $st->execute(["link_id" => $link_id]);
    }
    catch (Exception $e) {
        return NULL;
    }

    return $st;
}

function deleteFirstCommand($db, $commands_list) {
    try {
        $element = $commands_list->fetch();
        $commands_list->closeCursor();
        $command_id = $element["id"];

        $st = $db->prepare("
DELETE FROM commands
WHERE commands.id = :id");
        $st->execute(["id" => $command_id]);
    }
    catch (Exception $e) {
        return "Could not delete command.";
    }

    return "";
}

function enqueueCommand($db, $link_id, $content) {
    try {
        $st = $db->prepare("
INSERT INTO commands
    (link_id, content)
VALUES
    (:link_id, :content)");

        $st->execute(["link_id" => $link_id, "content" => $_REQUEST["content"]]);
    }
    catch (Exception $e) {
        return "Could not add command.";
    }

    return "";
}
?>

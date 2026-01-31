<?php
function listMachines($db, $username) {
    $st = $db->prepare("
SELECT machines.*, links.machine_id
FROM machines
JOIN links
ON links.machine_id = machines.id
JOIN users
ON links.user_id = users.id
WHERE users.username = :username
");
    $st->execute(["username" => $username]);

    return $st;
}

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
?>

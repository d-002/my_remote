<?php
function listMachines($db, $username) {
    $st = $db->prepare("
SELECT machines.*, links.name
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
SELECT sub.* FROM (
    SELECT commands.*
    FROM commands
    WHERE commands.link_id = :link_id
    ORDER BY commands.id DESC
    LIMIT 50
) as sub
ORDER BY sub.id ASC");

        $st->execute(["link_id" => $link_id]);
    }
    catch (Exception $e) {
        echo $e->getMessage();
        return NULL;
    }

    return $st;
}

function renameMachine($db, $user_hash, $machine_hash, $name) {
    $link_id = getUserMachineLink($db, $user_hash, $machine_hash);

    if ($link_id === NULL) {
        return "Could not find link.";
    }

    try {
        $st = $db->prepare("
UPDATE links
SET name = :name
WHERE links.id = :link_id");
        $st->execute(["name" => $name, "link_id" => $link_id]);
    }
    catch (Exception $e) {
        return "Could not rename machine.";
    }

    return "";
}

function getMachineState($db, $machine_hash) {
    try {
        $st = $db->prepare("
SELECT
    CASE
        WHEN (:time - machines.last_heartbeat) < 15 THEN machines.state
        ELSE 'offline'
    END AS state
FROM machines
WHERE machines.hash = :hash");
        $st->execute(["hash" => $machine_hash, "time" => time()]);
    }
    catch (Exception $e) {
        return NULL;
    }

    $state = $st->fetchColumn();
    $st->closeCursor();

    return $state === false ? NULL : $state;
}

function removeLink($db, $user_hash, $machine_hash) {
    $link_id = getUserMachineLink($db, $user_hash, $machine_hash);

    if ($link_id === NULL) {
        return "Could not find link.";
    }

    try {
        $st = $db->prepare("
DELETE FROM links
WHERE links.id = :link_id");
        $st->execute(["link_id" => $link_id]);
    }
    catch (Exception $e) {
        return "Failed to delete link.";
    }

    // check if no user references this machine, if so delete it
    try {
        $st = $db->prepare("
SELECT links.id
FROM links
JOIN machines
ON links.machine_id = machines.id
WHERE machines.hash = :hash");
        $st->execute(["hash" => $machine_hash]);
        $referenced = $st->fetchColumn() !== false;

        if (!$referenced) {
            try {
                $st = $db->prepare("
DELETE FROM machines
WHERE machines.hash = :hash");
                $st->execute(["hash" => $machine_hash]);
            }
            catch (Exception $e) {
                return "Failed to delete machine from database.";
            }
        }
    }
    catch (Exception $e) {
        return "Failed to check machine references.";
    }

    return "";
}
?>

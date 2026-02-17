<?php
function getSoftware($db, $user_hash) {
    try {
        $st = $db->prepare("
SELECT software.*
FROM software
JOIN users
ON software.user_id = users.id
WHERE users.hash = :hash");
        $st->execute(["hash" => $user_hash]);
        $line = $st->fetch();
        $st->closeCursor();
    }
    catch (Exception $e) {
        return NULL;
    }

    return $line === false ? NULL : $line;
}

function updateMachineHeartbeat($db, $machine_hash, $state) {
    try {
        $st = $db->prepare("
UPDATE machines
SET
    state = :state,
    last_heartbeat = :timestamp
WHERE machines.hash = :hash");
        $st->execute(["timestamp" => time(), "hash" => $machine_hash,
                      "state" => $state]);
    }
    catch (Exception $e) {
        return "Failed to update machine last heartbeat";
    }

    return "";
}
?>

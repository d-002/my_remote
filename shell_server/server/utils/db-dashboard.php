<?php
function listMachines($db, $username) {
    $st = $db->prepare("
SELECT machines.*
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
?>

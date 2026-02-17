<?php
function getVersionInfo($db, $user_hash) {
    try {
        $st = $db->prepare("
SELECT
    software.version,
    software.force_update
FROM software
JOIN users
ON software.user_id = users.id
WHERE users.hash = :hash");
        $st->execute(["hash" => $user_hash]);
        $software = $st->fetch();
        $st->closeCursor();
        if ($software === false) {
            return NULL;
        }

        $server_version = file_get_contents($_SERVER["DOCUMENT_ROOT"] . "/software/version");
        if ($server_version === false) {
            return NULL;
        }

        return ["user" => $software["version"], "server" => $server_version,
            "force" => $software["force_update"]];
    }
    catch (Exception $e) {
        return NULL;
    }
}

function syncUpdate($db, $user_hash) {
    $version = file_get_contents($_SERVER["DOCUMENT_ROOT"] . "/software/version");
    if ($version === false) {
        return "Could not read current software version.";
    }
    $version = trim($version);

    $binary = file_get_contents($_SERVER["DOCUMENT_ROOT"] . "/software/binary");
    if ($binary === false) {
        return "Could not read current software binary.";
    }

    try {
        $db->beginTransaction();
        $st = $db->prepare("
UPDATE software
SET version = :version, binary = :binary
WHERE software.user_id = (
    SELECT users.id
    FROM users
    WHERE users.hash = :hash
)");
        $st->execute(["version" => $version, "binary" => $binary, "hash" => $user_hash]);
        $db->commit();
    }
    catch (Exception $e) {
        $db->rollback();
        return "Failed to sync update.";
    }

    return "";
}

function toggleForceUpdate($db, $user_hash) {
    try {
        $db->beginTransaction();
        $st = $db->prepare("
UPDATE software
SET force_update = 1 - force_update
WHERE software.user_id = (
    SELECT users.id
    FROM users
    WHERE users.hash = :hash
)");
        $st->execute(["hash" => $user_hash]);
        $db->commit();
    }
    catch (Exception $e) {
        $db->rollback();
        return "Failed to sync update.";
    }

    return "";
}
?>

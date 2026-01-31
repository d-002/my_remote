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

function respondHeartbeat($db, $user_hash, $machine_hash, $content) {
    $lines = explode('\n', trim($content));

    $status = $lines[0];
    $machine_version = $lines[1];
    $version = $content;

    $software = getSoftware($db, $user_hash);
    if ($software === NULL) {
        return "error: Could not fetch user software version";
    }
    $user_version = $software->version;

    if (version_compare($version, $user_version) < 0) {
        try {
            $st = $db->prepare("
SELECT software.version
FROM software
JOIN users
ON software.user_id = users.id
WHERE users.hash = :hash");
            $st->execute(["hash" => $user_hash]);
            $binary = $st->fetch();
            $st->closeCursor();
        }
        catch (Exception $e) {
            return "error: Could not get user version file";
        }

        return $user_version . "\n" . $binary;
    }
    else {
        return "success";
    }
}
?>

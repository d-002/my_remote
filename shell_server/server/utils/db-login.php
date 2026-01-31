<?php
function fetchUser($db, $username) {
    try {
        $st = $db->prepare("SELECT * FROM users WHERE username = :username");
        $st->execute(["username" => $username]);
        $res = $st->fetch();
        $st->closeCursor();

        return $res === false ? NULL : $res;
    }
    catch (Exception $e) {
        return NULL;
    }
}

function registerUser($db, $username, $password) {
    $hash = password_hash($password, PASSWORD_DEFAULT);

    $exists = fetchUser($db, $username) !== NULL;
    if ($exists) {
        return "User already exists.";
    }

    $version = file_get_contents($_SERVER["DOCUMENT_ROOT"] . "/software/version");
    if ($version === false) {
        return "Could not read current software version.";
    }
    $version = trim($version);

    $binary = file_get_contents($_SERVER["DOCUMENT_ROOT"] . "/software/binary");
    if ($binary === false) {
        return "Could not read current software binary.";
    }

    $userHash = hash("sha1", $username . microtime());

    try {
        $db->beginTransaction();
        $st = $db->prepare("
INSERT INTO users
    (username, password, hash)
VALUES
    (:username, :password, :hash)");
        $st->execute(["username" => $username, "password" => $hash, "hash" => $userHash]);
        $user_id = $db->lastInsertId();

        $st = $db->prepare("
INSERT INTO software
    (user_id, version, binary)
VALUES
    (:user_id, :version, :binary)");
        $st->execute(["user_id" => $user_id, "version" => $version, "binary" => $binary]);
        $db->commit();
    }
    catch (Exception $e) {
        $db->rollback();
        return "Could not write to database, please try again.";
    }

    return "";
}

function loginUser($db, $username, $password) {
    $user = fetchUser($db, $username);

    if ($user === NULL) {
        return "No such user.";
    }

    if (!password_verify($password, $user["password"])) {
        return "Invalid password.";
    }

    return "";
}

function registerMachine($db, $user, $ip) {
    $hash = hash("sha1", $user["username"] . microtime());

    try {
        $db->beginTransaction();
        $st = $db->prepare("
INSERT INTO machines
    (hash)
VALUES
    (:hash)");
        $st->execute(["hash" => $hash]);

        $st = $db->prepare("
SELECT machines.id
FROM machines
WHERE machines.hash = :hash
ORDER BY machines.id DESC
LIMIT 1");
        $st->execute(["hash" => $hash]);
        $machine_id = $st->fetchColumn();
        $st->closeCursor();

        $st = $db->prepare("
INSERT INTO links
    (user_id, machine_id, name)
VALUES
    (:user_id, :machine_id, :ip)");
        $st->execute(["user_id" => $user["id"], "machine_id" => $machine_id, "ip" => $ip]);

        $db->commit();
    }
    catch (Exception $e) {
        $db->rollBack();
        return NULL;
    }

    return $hash;
}
?>

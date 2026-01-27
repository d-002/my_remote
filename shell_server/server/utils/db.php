<?php
function getDB() {
    $db = new PDO("sqlite:" . $_SERVER["DOCUMENT_ROOT"] . "/private/db.db");
    $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $db->setAttribute(PDO::ATTR_PERSISTENT, true);

    // check if the database is not empty and format it if necessary
    $sql = $db->query("
SELECT count(*)
FROM sqlite_master
WHERE type='table'
    AND name NOT LIKE 'sqlite_%'");

    $empty = $sql->fetchColumn() === 0;
    $sql->closeCursor();
    if ($empty) {
        resetDB($db);
    }

    return $db;
}

function resetDB($db) {
    $db->exec("DROP TABLE IF EXISTS users");
    $db->exec("DROP TABLE IF EXISTS machines");
    $db->exec("DROP TABLE IF EXISTS links");
    $db->exec("DROP TABLE IF EXISTS commands");
    $db->exec("
CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT,
    password TEXT,
    hash TEXT
)");
    $db->exec("
CREATE TABLE machines (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    hash TEXT
)");
    $db->exec("
CREATE TABLE links (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER,
    machine_id INTEGER,
    FOREIGN KEY(user_id) REFERENCES users(id),
    FOREIGN KEY(machine_id) REFERENCES machines(id)
)");
    $db->exec("
CREATE TABLE commands (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    link_id INTEGER,
    content TEXT,
    FOREIGN KEY(link_id) REFERENCES links(id)
)");
}

function fetchUser($db, $username) {
    $st = $db->prepare("SELECT * FROM users WHERE username = :username");
    $st->execute(["username" => $username]);
    $res = $st->fetch();
    $st->closeCursor();
    return $res;
}

function registerUser($db, $username, $password) {
    $hash = password_hash($password, PASSWORD_DEFAULT);

    $exists = fetchUser($db, $username) !== false;
    if ($exists) {
        return "User already exists.<br>";
    }

    $userHash = hash("sha256", $username . microtime());

    $st = $db->prepare("
INSERT INTO users
    (username, password, hash)
VALUES
    (:username, :password, :hash)");
    $st->execute(["username" => $username, "password" => $hash, "hash" => $userHash]);

    return NULL;
}

function loginUser($db, $username, $password) {
    $user = fetchUser($db, $username);

    if ($user === false) {
        return "No such user.<br>";
    }

    if (!password_verify($password, $user["password"])) {
        return "Invalid password.<br>";
    }

    return NULL;
}

function registerMachine($db, $user) {
    $hash = hash("sha256", $user["username"] . microtime());

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
    (user_id, machine_id)
VALUES
    (:user_id, :machine_id)");
    $st->execute(["user_id" => $user["id"], "machine_id" => $machine_id]);

    return $hash;
}
?>

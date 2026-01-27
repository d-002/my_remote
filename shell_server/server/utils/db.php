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

    if ($sql->fetchColumn() === 0) {
        resetDB($db);
    }

    return $db;
}

function resetDB($db) {
    $db->query("DROP TABLE IF EXISTS users");
    $db->query("DROP TABLE IF EXISTS machines");
    $db->query("DROP TABLE IF EXISTS links");
    $db->query("
CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT,
    password TEXT,
    hash TEXT
)");
    $db->query("
CREATE TABLE machines (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    hash TEXT
)");
    $db->query("
CREATE TABLE links (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER,
    machine_id INTEGER,
    FOREIGN KEY(user_id) REFERENCES users(id),
    FOREIGN KEY(machine_id) REFERENCES machines(id)
)");
}

function fetchUser($db, $username) {
    $st = $db->prepare("SELECT * FROM users WHERE username = :username");
    $st->execute(["username" => $username]);
    return $st->fetch();
}

function registerUser($db, $username, $password_CLEAR) {
    $hash = password_hash($password_CLEAR, PASSWORD_DEFAULT);

    $exists = fetchUser($db, $username) !== false;
    if ($exists) {
        return "User already exists.<br>";
    }

    $userHash = hash("sha256", $username . time());

    $st = $db->prepare("
INSERT INTO users
    (username, password, hash)
VALUES
    (:username, :password, :hash)");
    $st->execute(["username" => $username, "password" => $hash, "hash" => $userHash]);

    return NULL;
}

function loginUser($db, $username, $password_CLEAR) {
    $user = fetchUser($db, $username);

    if ($user === false) {
        return "No such user.<br>";
    }

    if (!password_verify($password_CLEAR, $user["password"])) {
        return "Invalid password.<br>";
    }

    return NULL;
}
?>

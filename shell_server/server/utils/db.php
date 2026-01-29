<?php
function getDB() {
    $db = new PDO("sqlite:" . $_SERVER["DOCUMENT_ROOT"] . "/private/db.db");
    $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $db->setAttribute(PDO::ATTR_PERSISTENT, true);
    $db->setAttribute(PDO::ATTR_TIMEOUT, 1);

    // check if the database is not empty and format it if necessary
    try {
        $db->beginTransaction();
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
        $db->commit();
    }
    catch (Exception $e) {
        $db->rollback();
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
    hash TEXT,
    name TEXT
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
?>

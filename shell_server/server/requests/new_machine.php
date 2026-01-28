<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");

function getText() {
    $username = $_REQUEST["username"];
    $password = $_REQUEST["password"];

    $db = getDB();
    $err = loginUser($db, $username, $password);
    if ($err !== "") {
        return "error: " . $err;
    }

    $user = fetchUser($db, $username);
    if ($user === NULL) {
        return "error: Could not fetch user after login.";
    }

    $hash = registerMachine($db, $user);
    if ($hash === NULL) {
        return "error: Could not register machine.";
    }

    return '{"user_hash":"' . $user["hash"] . ',"machine_hash":"' . $hash . '"}';
}

echo getText();

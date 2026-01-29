<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-login.php");

$db = getDB();
$error = loginUser($db, $_REQUEST["username"], $_REQUEST["password"]);

if ($error === "") {
    $user = fetchUser($db, $_REQUEST["username"]);

    if ($user === NULL) {
        $error = "Could not fetch user after login.";
    }
}

if ($error === "") {
    $hash = registerMachine($db, $user);
    if ($hash === NULL) {
        $error = "Could not register machine.";
    }
}

echo $error === "" ? '{"user_hash":"' . $user["hash"] . ',"machine_hash":"' . $hash . '"}' : "error: " . $error;

<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");

$username = $_REQUEST["username"];
$password = $_REQUEST["password"];

$db = getDB();
$err = loginUser($db, $username, $password);

if ($err == "") {
    $user = fetchUser($db, $username);
    $hash = registerMachine($db, $user);

    echo '{"user_hash":"' . $user["hash"] . ',"machine_hash":"' . $hash . '"}';
}

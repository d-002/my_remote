<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-dashboard.php");

session_start();

$db = getDB();
?>

<html>
<head>
<?php
if ($_SESSION["username"] === NULL) {
?>
    <meta http-equiv="refresh" content="0; url=/login">
<?php
}
?>
    <title>shell_server - Dashboard</title>
</head>

<body>
<div>
    <h1>Welcome, <?= $_SESSION["username"] ?></h1>
    <p>My machines</p>
    <ul>
<?php
try {
    $st = listMachines($db, $_SESSION["username"]);

    while (true) {
        $current = $st->fetch();
        if ($current === false) {
            break;
        }

        echo '<li machine_hash="' . $current["hash"] . '" onclick="select(this)">';
        echo $current["name"];
        echo "</li>";
    }
    $st->closeCursor();
}
catch (Exception $e) {
    echo '<p class="error">Failed to load machines: ' . $e->getMessage() . "</p>\n";
}
?>
    </ul>
</div>
</body>
</html>

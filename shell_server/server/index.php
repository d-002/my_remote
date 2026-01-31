<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-login.php");
my_include("/utils/db-machines.php");

session_start();

$db = getDB();
$logged_in = $_SESSION["username"] !== NULL;

if ($logged_in) {
    $user_hash = fetchUser($db, $_SESSION["username"])["hash"];
}
?>

<html>
<head>
<?php
if (!$logged_in) {
?>
    <meta http-equiv="refresh" content="0; url=/login">
    <style>#only-logged-in { display: none; }</style>
<?php
}
?>
    <title>shell_server - Dashboard</title>
</head>

<body user-hash="<?= $user_hash ?>">
<div id="only-logged-in">
    <h1>Welcome, <?= $_SESSION["username"] ?></h1>
    <p>My machines</p>
    <ul id="machines-list">
<?php
try {
    $st = listMachines($db, $_SESSION["username"]);

    while (true) {
        $current = $st->fetch();
        if ($current === false) {
            break;
        }

        echo '<li machine-hash="' . $current["hash"] . '">';
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

    <div id="selected-machine">
        <p>Selected machine</p>
        <p id="selected-machine-info"</p>
        <p>Shell</p>
        <div id="shell"></div>
        <form action="javascript:sendCommand()">
            <label for="shell-input">$</label>
            <input type="text" id="shell-input">
        </form>

        <a onclick="clearHistory()">Clear history</a>
        <a onclick="deselect()">Deselect machine</a>
    </div>

</div>
</body>

<script src="/scripts/utils.js"></script>
<script src="/scripts/dashboard.js"></script>
</html>

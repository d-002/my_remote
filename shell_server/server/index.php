<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-login.php");
my_include("/utils/db-machines.php");

session_start();

$db = getDB();
$logged_in = $_SESSION["username"] !== NULL;

if ($logged_in) {
    $user = fetchUser($db, $_SESSION["username"]);
    $user_hash = $user["hash"];
}
?>

<html>
<head>
<?php
if (!$logged_in) {
    // go to login page if not logged in
?>
    <meta http-equiv="refresh" content="0; url=/login">
<?php
}
else if ($user === NULL) {
    // user got deleted
?>
    <meta http-equiv="refresh" content="0; url=/logout">
<?php
}
?>
    <title>shell_server - Dashboard</title>

    <link rel="stylesheet" href="/styles/common.css">
    <link rel="stylesheet" href="/styles/dashboard.css">
    <meta name="viewport" content="width=device-width, initial-scale=1">
</head>

<body username="<?= $_SESSION["username"] ?>" user-hash="<?= $user_hash ?>">

<div id="header">
    <div id="header-left">
    </div>

    <div id="header-center">
        <h1>Welcome, <?= $_SESSION["username"] ?></h1>
    </div>

    <div id="header-right">
        <a href="/logout">Log out</a>
    </div>
</div>

<div id="main">
<div id="left">
    <h2>My machines</h2>
    <ul id="machines-list">
<?php
$hasMachines = false;
try {
    $st = listMachines($db, $_SESSION["username"]);

    while (true) {
        $hasMachines = true;
        $current = $st->fetch();
        if ($current === false) {
            break;
        }

        echo '        <li machine-hash="' . $current["hash"] . '">';
        echo $current["name"];
        echo "</li>\n";
    }
    $st->closeCursor();
}
catch (Exception $e) {
    echo '<p class="error">Failed to load machines: ' . $e->getMessage() . "</p>\n";
}
?>
    </ul>

    <p class="small"><?= $hasMachines ?
        "Use the installer to add more machines."
        : "You have no machines.<br>Use the installer to add one." ?></p>
</div>

<div id="right">
    <h3>Selected machine</h3>
    <p id="selected-machine-info"</p>
    <h3>Shell</h3>
    <div id="shell"></div>
    <form id="command-form" action="javascript:sendCommand()">
        <label for="shell-input">$</label>
        <input type="text" id="shell-input" placeholder="Type a command...">
    </form>

    <h3>Actions</h3>
    <div id="actions">
        <a onclick="renameMachine()">Rename machine</a>
        <a onclick="restartMachine()">Restart machine</a>
        <a onclick="clearShell()">Clear shell history</a>
        <a onclick="destroyMachine()">Ask for machine destruction</a>
        <a onclick="destroyLink()">Destroy link to machine</a>
        <a onclick="deleteAccount()">Delete account</a>
    </div>
</div>
</div>

</body>

<script src="/scripts/http-utils.js"></script>
<script src="/scripts/state.js"></script>
<script src="/scripts/dashboard.js"></script>
</html>

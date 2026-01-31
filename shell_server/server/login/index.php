<?php
require $_SERVER["DOCUMENT_ROOT"] . "/utils/base.php";
my_include("/utils/db.php");
my_include("/utils/db-login.php");

startsess();
$signup = $_GET["signup"] !== NULL;

// check form submission
$error = "";
$redirect = $_SESSION["username"] !== NULL;

if (count($_POST) !== 0) {
    $username = $_POST["username"];
    $password = $_POST["password"];
    $password2 = $_POST["confirm-password"];

    $len = strlen($username);
    if ($len === 0) {
        $error .= "Cannot have an empty username.<br>";
    }
    else if ($len > 32) {
        $error .= "Username too long.<br>";
    }

    if ($signup) {
        $len = strlen($password);
        if ($len < 8) {
            $error .= "Password should be at least 8 characters.<br>";
        }
        else if ($len > 256) {
            $error .= "Password too long.<br>";
        }

        if ($password !== $password2) {
            $error .= "Passwords don't match.<br>";
        }
    }

    if ($error === "") {
        $db = getDB();
        $err = $signup ? registerUser($db, $username, $password)
                       : loginUser($db, $username, $password);

        if ($err === "") {
            $_SESSION["username"] = $username;
            $redirect = true;
        }
        else {
            $error .= $err . "<br>";
        }
    }
}
?>

<html>
<head>
    <title>shell_server - <?= $signup ? "Sign up" : "Log in" ?></title>
    <?php
    if ($redirect) {
    ?>
    <meta http-equiv="refresh" content="0; url=/">
    <?php
    }
    ?>
    <link rel="stylesheet" href="/styles/common.css">
    <link rel="stylesheet" href="/styles/login.css">
</head>
<body class="center-flex">
<div id="main">
    <form method="POST">
        <div>
            <label for="username">Username</label>
            <input type="text" id="username" name="username" value="<?= $username ?>">
        </div>
        <div>
            <label for="password">Password</label>
            <input type="password" id="password" name="password">
        </div>

        <?php
        if ($signup) {
        ?>
        <div>
            <label for="confirm-password">Confirm password</label>
            <input type="password" id="confirm-password" name="confirm-password">
        </div>
        <?php
        }
        ?>

        <input type="submit" value="Submit">
    </form>

    <?php
    if ($error !== "") {
    ?>
    <p class="small error"><?= $error ?> </p>
    <?php
    }
    ?>

    <?php
    if ($signup) {
    ?>
    <p class="small">Already have an account?</p><a href="/login">Log in</p>
    <?php
    }
    else {
    ?>
    <p class="small">Don't have an account?</p><a href="/login?signup">Sign up</p>
    <?php
    }
    ?>
</div>
</body>
</html>

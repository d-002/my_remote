<?php
function my_include($name) {
    include_once $_SERVER["DOCUMENT_ROOT"] . $name;
}

function startsess() {
    if (session_status() === PHP_SESSION_NONE)
        session_start();
}
?>

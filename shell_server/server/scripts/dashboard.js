// ===== variables =====

const username = document.body.getAttribute("username");
const user_hash = document.body.getAttribute("user-hash");
const info = document.getElementById("selected-machine-info");
const machinesList = document.getElementById("machines-list");
const shell = document.getElementById("shell");
const command = document.getElementById("shell-input");

const selected = {
    // < 0 when nothing is selected
    index: machinesList.children.length === 0 ? -1 : 0,
    hash: null,
};

let lastCommand;

// ===== functions =====

function updateStates() {
    Array.from(machinesList.children).forEach(elt => {
        const hash = elt.getAttribute("machine-hash");
        get("/api/get_machine_state.php?machine=" + hash, text => {
            if (text.startsWith("error")) {
                alert(text);
                return;
            }

            text = text.trim();
            const base = elt.classList.contains("selected") ? "selected " : "";
            elt.className = base + text;
            elt.title = "This machine is " + text;
        });
    });
}

function getCommands(callback) {
    get("/api/list_commands.php?user=" + user_hash + "&machine="
        + selected.hash, callback);
}

function updateShell(text) {
    // text can be null, in this case compute and callback
    if (text == null) {
        return getCommands(updateShell);
    }

    if (selected.index < 0) {
        shell.innerHTML = "";
        return;
    }

    shell.innerHTML = "";

    if (text.startsWith("error")) {
        alert(text);
        return;
    }

    text.split("\n").forEach(command => {
        if (command === "") {
            return;
        }

        command = command.replaceAll(String.fromCharCode(0x7), "\n");

        const colonIndex = command.indexOf(":");
        const words = command.substring(0, colonIndex).split(" ");
        const who = words[0], read_state = words[1];
        const status = words[2], timestamp = words[2];
        let message = command.substring(colonIndex + 1);

        // normal message, careful about encoded newlines and how they are
        // displayed
        if (status === "normal") {
            let pending = false;
            if (who === "user") {
                message = "$ " + message;
                if (read_state === "pending") {
                    pending = true;
                }
            }
            message.split("\n").forEach(line => {
                const p = document.createElement("p");

                p.textContent += line;
                if (pending) {
                    p.className = "pending";
                    p.title = "[not received] ";
                }
                p.title += new Date(parseInt(timestamp));
                shell.appendChild(p);
            });
        }
        // a status report from the machine
        else {
            const p = document.createElement("p");
            p.className = "special";

            if (status === "report") {
                p.textContent = "[machine report]: " + message;
            }
            // a custom request from the user
            else if (status === "request") {
                p.textContent = "[user request]: " + message;
            }
            else {
                p.textContent = "[unknown status]: " + message;
            }

            shell.appendChild(p);
        }
    });

    shell.scrollTop = shell.scrollHeight;
}

function updateSelection() {
    if (selected.index >= 0) {
        const elt = machinesList.children[selected.index];
        selected.hash = elt.getAttribute("machine-hash");
        info.innerHTML = "<code>" + selected.hash + "</code>";
    }

    let i = 0;
    Array.from(machinesList.children).forEach(
        elt => {
            if (i++ === selected.index) {
                elt.classList.add("selected");
            }
            else {
                elt.classList.remove("selected");
            }
        });
}

function sendCommand() {
    if (selected.index < 0) {
        alert("error: Please select a machine first.");
        return;
    }

    const content = command.value;
    post("/api/enqueue_command.php?user=" + user_hash + "&machine="
        + selected.hash + "&is_user", content, text => {
        if (text.startsWith("error")) {
            alert(text);
        }
        else {
            updateShell();
            command.value = "";
            stateUpdate(true);
        }
    });
}

function renameMachine() {
    if (selected.index < 0) {
        alert("error: Please select a machine first.");
        return;
    }

    const oldName = machinesList.children[selected.index].textContent;
    let newName = prompt("Enter new machine name: (previous was '" +
        oldName + "')");
    if (newName === null) {
        return;
    }
    newName = newName.trim();
    if (newName.length === 0) {
        alert("Empty name");
        return;
    }

    get("/api/rename_machine.php?user=" + user_hash + "&machine="
        + selected.hash + "&name=" + newName, text => {
        if (text.startsWith("error")) {
            alert(text);
        }
        else {
            machinesList.children[selected.index].textContent = newName;
            updateSelection();
        }
    });
}

function clearShell() {
    if (selected.index < 0) {
        alert("error: Please select a machine first.");
        return;
    }

    const ans = confirm("Really clear all shell history, even unread commands?");
    if (ans !== true) {
        return;
    }

    get("/api/clear_shell.php?user=" + user_hash + "&machine="
        + selected.hash, text => {
        if (text.startsWith("error")) {
            alert(text);
        }
        else {
            updateShell();
        }
    });
}

function restartMachine() {
    const ans = confirm("Really restart the machine?");
    if (ans !== true) {
        return;
    }

    post("/api/enqueue_command.php?user=" + user_hash + "&machine="
        + selected.hash + "&is_user&status=request", "restart", text => {
        if (text.startsWith("error")) {
            alert(text);
        }
        else {
            updateShell();
            command.value = "";
            stateUpdate(true);
        }
    });
}

function destroyMachine() {
    const ans = confirm("Really stop and destroy shell files on the machine?");
    if (ans !== true) {
        return;
    }

    post("/api/enqueue_command.php?user=" + user_hash + "&machine="
        + selected.hash + "&is_user&status=request", "destroy", text => {
        if (text.startsWith("error")) {
            alert(text);
        }
        else {
            updateShell();
            command.value = "";
            stateUpdate(true);
        }
    });
}

function destroyLink() {
    const ans = confirm("Really unlink the machine with this user?\n" +
        "If the shell is still running, this will not stop it.");
    if (ans !== true) {
        return;
    }

    post("/api/destroy_link.php?user=" + user_hash + "&machine="
        + selected.hash, null, text => {
        if (text.startsWith("error")) {
            alert(text);
        }
        else {
            document.location.reload();
        }
    });
}

function deleteAccount() {
    const ans = confirm("Really delete this account?\n" +
        "Running machines will not be stopped.");
    if (ans !== true) {
        return;
    }

    const password = prompt("Enter your password:");
    if (password == null) {
        return;
    }

    post("/api/delete_user.php?username=" + username + "&password=" + password,
        null, text => {
            if (text.startsWith("error")) {
                alert(text);
            }
            else {
                document.location.href = "/login";
            }
        });
}

function syncUpdate() {
    post("/api/sync_update.php?user=" + user_hash, null, text => {
        if (text.startsWith("error")) {
            alert(text);
        }
        else {
            document.location.reload();
        }
    });
}

function toggleForceUpdate() {
    post("/api/toggle_force_update.php?user=" + user_hash, null, text => {
        if (text.startsWith("error")) {
            alert(text);
        }
        else {
            document.location.reload();
        }
    });
}

// ===== listeners =====

updateSelection();

machinesList.addEventListener("click", evt => {
    if (evt.target.tagName.toUpperCase() === "LI") {
        const children = Array.from(evt.target.parentNode.children);

        selected.index = children.indexOf(evt.target);
        updateSelection();
        updateStates();
        updateShell();
    }
});

startStateInterval(callback => {
    getCommands(text => {
        updateStates();

        const textTrimmed = text.trimEnd();
        const last = textTrimmed.substring(textTrimmed.lastIndexOf("\n") + 1);

        if (last === lastCommand) {
            return callback(false);
        }

        console.log("Detected new commands, refreshing");
        updateShell(text);
        lastCommand = last;
        return callback(true);
    });
});

// ===== variables =====

const user_hash = document.body.getAttribute("user-hash");
const info = document.getElementById("selected-machine-info");
const machinesList = document.getElementById("machines-list");
const shell = document.getElementById("shell");
const command = document.getElementById("shell-input");

const selected = {
    index: 0, // < 0 when nothing is selected
    hash: null,
};

let lastCommand;

// ===== functions =====

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
        const who = words[0], state = words[1], timestamp = words[2];
        let message = command.substring(colonIndex + 1);

        let pending = false;
        if (who === "user") {
            message = "$ " + message;
            if (state === "pending") {
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
        elt => elt.className = i++ === selected.index ? "selected" : "");
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

function clearShell() {
    if (selected.index < 0) {
        alert("error: Please select a machine first.");
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

function renameMachine() {
    if (selected.index < 0) {
        alert("error: Please select a machine first.");
        return;
    }

    const oldName = machinesList.children[selected.index].textContent;
    const newName = prompt("Enter new machine name: (previous was '" +
        oldName + "')");

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

// ===== listeners =====

updateSelection();

machinesList.addEventListener("click", evt => {
    if (evt.target.tagName.toUpperCase() === "LI") {
        const children = Array.from(evt.target.parentNode.children);

        selected.index = children.indexOf(evt.target);
        updateSelection();
        updateShell();
    }
});

startStateInterval(callback => {
    getCommands(text => {
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

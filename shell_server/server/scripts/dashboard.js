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

// ===== functions =====

function updateShell() {
    if (selected.index < 0) {
        shell.innerHTML = "";
        return;
    }

    fetch("/api/list_commands.php?user=" + user_hash + "&machine="
        + selected.hash, text => {
        shell.innerHTML = "";

        text.split("\n").forEach(line => {
            if (line === "") {
                return;
            }

            const p = document.createElement("p");
            p.textContent = line;
            shell.appendChild(p);
        });
    });
}

function updateSelection() {
    if (selected.index >= 0) {
        const elt = machinesList.children[selected.index];
        selected.hash = elt.getAttribute("machine-hash");
        info.innerHTML = "hash: <code>" + selected.hash + "</code>";
    }

    let i = 0;
    Array.from(machinesList.children).forEach(
        elt => elt.className = i++ == selected.index ? "selected" : "");
}

function sendCommand() {
    if (selected.index < 0) {
        alert("error: Please select a machine first.");
        return;
    }

    const content = command.value;
    fetch("/api/enqueue_command.php?user=" + user_hash + "&machine="
        + selected.hash + "&is_user&content=" + content, text => {
            if (text.startsWith("error")) {
                alert(text);
            }
            else {
                updateShell();
                command.value = "";
            }
    });
}

function clearShell() {
    if (selected.index < 0) {
        alert("error: Please select a machine first.");
        return;
    }

    fetch("/api/clear_shell.php?user=" + user_hash + "&machine="
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

    fetch("/api/rename_machine.php?user=" + user_hash + "&machine="
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

machinesList.addEventListener("click", evt => {
    if (evt.target.tagName.toLowerCase() == "li") {
        const children = Array.from(evt.target.parentNode.children);

        selected.index = children.indexOf(evt.target);
        updateSelection();
        updateShell();
    }
});

// ===== main =====

updateSelection();
updateShell();

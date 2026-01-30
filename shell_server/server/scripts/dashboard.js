// ===== variables =====

const user_hash = document.body.getAttribute("user-hash");
const info = document.getElementById("selected-machine-info");
const machinesList = document.getElementById("machines-list");
const shell = document.getElementById("shell");
const command = document.getElementById("shell-input");

const selected = {
    index: -1, // < 0 when nothing is selected
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

function showSelection() {
    if (selected.index >= 0) {
        const name = machinesList.children[selected.index].textContent;
        info.textContent = "name: " + name + ", hash: " + selected.hash;
    }

    let i = 0;
    Array.from(machinesList.children).forEach(
        elt => elt.className = i++ == selected.index ? "selected" : "");
}

function deselect() {
    selected.index = -1;
    selected.hash = null;

    showSelection();
    updateShell();
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

// ===== listeners =====

machinesList.addEventListener("click", evt => {
    if (evt.target.tagName.toLowerCase() == "li") {
        const children = Array.from(evt.target.parentNode.children);

        // update selection variable
        selected.index = children.indexOf(evt.target);
        selected.hash = evt.target.getAttribute("machine-hash");

        // update selection visually
        showSelection();
        updateShell();
    }
});

// ===== main =====

showSelection();
updateShell();

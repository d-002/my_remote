// time in seconds
const SLEEP_IDLE = 10;
const SLEEP_ACTIVE = 1;
const IDLE_THRESHOLD = 15;

const IDLE = 0;
const ACTIVE = 1;

let state = IDLE;
let lastActionTimestamp = 0;

function stateUpdate(action) {
    const nowSeconds = Date.now() / 1000;

    if (action) {
        lastActionTimestamp = nowSeconds;
    }

    const prev = state;
    state = nowSeconds - lastActionTimestamp < IDLE_THRESHOLD ? ACTIVE : IDLE;

    if (prev != state) {
        const stateString = state == ACTIVE ? "active" : "idle";
        console.log("Switching state to " + stateString);
    }
}

// run the callback, which calls this function back again with a result, used
// to determine whether to change state
function loop(callback) {
    callback(action => {
        stateUpdate(action);

        const delay = state == ACTIVE ? SLEEP_ACTIVE : SLEEP_IDLE;

        return self.setTimeout(() => loop(callback), delay * 1000);
    });
}

function startStateInterval(callback) {
    return loop(callback);
}

import { writable } from "svelte/store";

export async function initSerialPort() {
    const port = await navigator.serial.requestPort();
}

export let refreshRate = writable(); 
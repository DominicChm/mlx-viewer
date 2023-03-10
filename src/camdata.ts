import { onMount } from "svelte";
import { subscribe } from "svelte/internal";
import { get, writable } from "svelte/store";

let portFinishFlag = false;
export async function initSerialPort() {
    const port = await navigator.serial.requestPort();
    try {
        await port.open({ baudRate: 1_500_000 });
        (port as any).setSignals({
            requestToSend: true,
        });

        await new Promise(r => setTimeout(r, 50));

        await (port as any).setSignals({
            dataTerminalReady: false,
            requestToSend: false,
        });
    } catch (e) {
        console.error(e);
        portError.set(e.toString());
        return;
    }

    let parser = new Parser();
    portFinishFlag = false;
    logs.set([]);
    image.set(null);

    portError.set(null);
    portOpen.set(true);
    try {
        while (port.readable) {
            const reader = port.readable.getReader();
            try {
                while (true) {
                    const { value, done } = await reader.read();
                    if (done || !get(portOpen)) {
                        reader.releaseLock();
                        return;
                    }
                    parser.push(value);
                }
            } catch (error) {
                console.error(error);
            }
        }
    } finally {
        console.log("READER FINISHED")
        port.close();
    }
    portError.set(null);
    portOpen.set(false);

}

export async function closeSerialPort() {
    portOpen.set(false);
}

export let timings = writable<{ t_frame_fetch: number, t_frame_tx_time: number, t_calc_time: number } | null>(null);
export let portError = writable<null | string>(null);
export let refreshRate = writable<number>();
export let portOpen = writable<boolean>();
export let logs = writable<{ idx: number, msg: string }[]>([]);
export let image = writable<Float32Array | null>(null);


class Parser {
    msg: DataView | null;
    cmd = 0;
    len = 0;

    header: DataView = new DataView(new ArrayBuffer(2))
    idx = 0;

    state = 0;
    num_rx = 0;

    logIdx = 0;

    constructor() {

    }


    push(data: Uint8Array) {
        for (const b of data) {
            this.parseFSM(b);
        }
    }

    parseFSM(byte: number) {
        switch (this.state) {
            case 0:
                if (byte == 0xA0)
                    this.state = 1;
                break;
            case 1:
                this.cmd = byte;
                this.state = 2;
                this.idx = 0;
                break;

            case 2:
                this.header.setUint8(this.idx++, byte);
                if (this.idx >= 2) {
                    if ((this.len = this.header.getUint16(0, true)) > 5000) {
                        console.log(`LONG PACKET ${this.len}`);
                        this.state = 0;
                    } else {
                        this.msg = new DataView(new ArrayBuffer(this.len));
                        this.state = 3;
                    }
                    this.idx = 0;
                }
                break;

            case 3:
                this.msg.setUint8(this.idx++, byte);
                if (this.idx >= this.len) {
                    this.interpretMessage()

                    // reset
                    this.idx = 0;
                    this.state = 0;
                    delete this.msg;
                }
                break;
        }
    }

    interpretMessage() {
        //console.log(this.cmd);

        switch (this.cmd) {
            case 0x00: // Image
                image.set(new Float32Array(this.msg.buffer));
                break;

            case 0x01: // Debug message
                let msg = new TextDecoder("ascii").decode(this.msg.buffer);
                logs.update(l => [...l, { idx: this.logIdx++, msg }])
                break;

            case 0x03: // Timings
                timings.set({
                    t_frame_fetch: this.msg.getInt32(0, true),
                    t_frame_tx_time: this.msg.getInt32(4, true),
                    t_calc_time: this.msg.getInt32(8, true),
                });
                break;
        }
    }

    reset() {

    }
}
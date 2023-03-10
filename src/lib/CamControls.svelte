<script lang="ts">
    import { onMount } from "svelte";
    import { invoke } from "@tauri-apps/api/tauri";
    import { closeSerialPort, initSerialPort, refreshRate, logs, timings, portOpen } from "../camdata";

    let ports = [];
    onMount(() => {
        //document.scrollingElement.scroll(0, 1);
    });
</script>

<div class="control-container">
    {#if !$portOpen}
        <button on:click={initSerialPort}>Open Port</button>
    {:else}
        <button on:click={closeSerialPort} class="connected">Close Port</button>
    {/if}
    {#if $timings}
        <h3>Timings</h3>
        <p>Calc time: {$timings.t_calc_time}</p>
        <p>Frame Fetch: {$timings.t_frame_fetch}</p>
        <p>TX time: {$timings.t_frame_tx_time}</p>
    {/if}
    <h3>Log</h3>
    <div class="logs-scroller">
        <div class="logs">
            {#each $logs as log (log.idx)}
                <pre>{log.idx} - {log.msg}</pre>
            {/each}
        </div>
    </div>
</div>

<style>
    .logs {
        color: black;
        overflow-y: visible;
        height: fit-content;
    }

    .logs-scroller {
        background-color: whitesmoke;
        overflow-y: scroll;
        display: flex;
        flex-direction: column-reverse;
        height: 10rem;
    }

    .logs * {
        margin: 0;
    }
    .connected {
        background-color: darkslategray;
    }
</style>

<script lang="ts">
    import { onMount } from "svelte";
    import { invoke } from "@tauri-apps/api/tauri";
    import { initSerialPort, refreshRate } from "../camdata";

    let ports = [];
</script>

<div>
    <button on:click={initSerialPort}>Open Port</button>
</div>

<style>
    canvas {
        width: 100%;
        max-height: 100%;
        image-rendering: pixelated;
        aspect-ratio: 32/24;
    }

    input {
        min-width: 0;
    }

    .comselect {
        display: grid;
        grid-template-columns: 1fr auto auto;
        width: 100%;
        gap: 0.25rem;
    }
</style>

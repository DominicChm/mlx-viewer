<script lang="ts">
    import { onMount } from "svelte";
    import { invoke } from "@tauri-apps/api/tauri";

    let ports = null;

    async function refreshPorts() {
        ports = await invoke("ports");
    }

    let canvas;
    let refreshrate = 1;

    onMount(() => {
        refreshPorts();
        
        let ctx: CanvasRenderingContext2D = canvas.getContext("2d");
        ctx.imageSmoothingEnabled = true;
        ctx.imageSmoothingQuality = "low";

        ctx.fillStyle = "darkred";
        ctx.fillRect(0, 0, 32, 24);

        ctx.fillStyle = "black";
        ctx.fillRect(0, 0, 1, 1);
    });
</script>

<div>
    <canvas bind:this={canvas} width="32" height="24" />
    <p>{refreshrate}</p>
    <input type="range" min={0} max={7} bind:value={refreshrate} />

    <button on:click={refreshPorts}>Refresh</button>
    {#if ports}
        <p>{JSON.stringify(ports)}</p>
    {:else}
        <p>No ports yet...</p>
    {/if}
</div>

<style>
    canvas {
        height: 100%;
        width: 100%;
        image-rendering: pixelated;
    }
</style>

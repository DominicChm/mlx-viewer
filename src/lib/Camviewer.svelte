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
</div>

<style>
    canvas {
        padding: 1rem;
        box-sizing: border-box;
        min-width: 0;
        min-height: 0;
        width: 100%;
        height: 100%;
        image-rendering: pixelated;
        aspect-ratio: 32/24;
        object-fit: contain;
    }
    div {
        height: 100%;
        overflow: hidden;
    }
</style>

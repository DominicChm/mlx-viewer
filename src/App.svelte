<script lang="ts">
  import Camviewer from "./lib/Camviewer.svelte";
  import Greet from "./lib/Greet.svelte";

</script>

<main class="container">
  <h1>MLX Viewer</h1>

  <Camviewer />
  <p>Click on the Tauri, Vite, and Svelte logos to learn more.</p>

  <div class="row">
    <Greet />
  </div>
</main>

<style>
  .logo.vite:hover {
    filter: drop-shadow(0 0 2em #747bff);
  }

  .logo.svelte:hover {
    filter: drop-shadow(0 0 2em #ff3e00);
  }
</style>

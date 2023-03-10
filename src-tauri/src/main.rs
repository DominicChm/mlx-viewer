// Prevents additional console window on Windows in release, DO NOT REMOVE!!
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]
use std::sync::Arc;
use tokio::sync::watch::{channel, Receiver, Sender};

#[derive(Debug)]
struct State {
    watch_debugln: Receiver<String>,
    watch_img: Receiver<Vec<u16>>,
}
// https://github.com/tauri-apps/tauri/commit/8b6f3de0ad47684e72a2ae5f884d8675acfaeeac
// Learn more about Tauri commands at https://tauri.app/v1/guides/features/command
#[tauri::command]
fn ports() -> Result<Vec<String>, String> {
    serialport::available_ports()
        .map_err(|e| e.to_string())
        .map(|s| s.into_iter().map(|x| x.port_name).collect())
}

#[tauri::command]
fn greet(name: &str) -> String {
    format!("Hello, {}! You've been greeted from Rust!", name)
}

#[tauri::command]
async fn get_debugln(
    argument: Option<String>,
    state: tauri::State<'_, State>,
) -> Result<String, String> {
    let mut w = state.watch_debugln.clone();
    if w.changed().await.is_ok() {
        Ok(w.borrow_and_update().to_string())
    } else {
        Err("RX error :(".to_string())
    }
}

#[tokio::main]
async fn main() {
    let (tx_debugln, watch_debugln) = channel("".to_string());
    let (tx_img, watch_img) = channel(Vec::new());

    tokio::spawn(async move {});

    tauri::Builder::default()
        .manage(State {
            watch_debugln: watch_debugln.clone(),
            watch_img: watch_img.clone(),
        })
        .invoke_handler(tauri::generate_handler![greet, get_debugln, ports])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

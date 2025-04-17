const keyOptions = [
  { label: 'Enter', value: '\r' },
  { label: 'Space', value: ' ' },
];
// Add letters A-Z
for (let c = 65; c <= 90; c++) {
  keyOptions.push({ label: String.fromCharCode(c), value: String.fromCharCode(c) });
}
// Add digits 0-9
for (let d = 48; d <= 57; d++) {
  keyOptions.push({ label: String.fromCharCode(d), value: String.fromCharCode(d) });
}

let port;
let reader;
let writer;
let textBuffer = '';
// 現在のマッピングを保持 (index->[codes])
let currentMapping = {};

// Connect to the serial device
async function connectDevice() {
  try {
    port = await navigator.serial.requestPort();
    await port.open({ baudRate: 115200 });
    writer = port.writable.getWriter();
    reader = port.readable.getReader();
    document.getElementById('connect').disabled = true;
    document.getElementById('refresh').disabled = false;
    document.getElementById('apply').disabled = false;
    readLoop();
    await refreshMapping();
  } catch (err) {
    console.error(err);
  }
}

// Read incoming data
async function readLoop() {
  while (port.readable) {
    const { value, done } = await reader.read();
    if (done) break;
    const text = new TextDecoder().decode(value);
    textBuffer += text;
    const lines = textBuffer.split(/\r?\n/);
    textBuffer = lines.pop();
    for (const line of lines) {
      processLine(line);
    }
  }
}

// Process a line from the device
function processLine(line) {
  const idx = line.indexOf(':');
  let cmd, payload;
  if (idx >= 0) {
    cmd = line.substring(0, idx);
    payload = line.substring(idx + 1);
  } else {
    const parts = line.split(' ');
    cmd = parts[0];
    payload = parts.slice(1).join(' ');
  }
  if (cmd === 'get') {
    // JSON形式: {0:[...],1:[...],...}
    let data;
    try {
      data = JSON.parse(payload);
    } catch (e) {
      console.error('Invalid JSON:', payload);
      return;
    }
    currentMapping = data;
    for (let i = 0; i < 7; i++) {
      const arr = data[i] || [];
      const select = document.getElementById(`key-${i}`);
      if (arr.length > 1) {
        select.value = '__random';
      } else {
        const code = arr[0] || 0;
        let found = false;
        for (let opt of select.options) {
          if (opt.value.charCodeAt(0) === code) {
            select.value = opt.value;
            found = true;
            break;
          }
        }
        if (!found) select.selectedIndex = 0;
      }
    }
    return;
  }
}

// Request current mapping
async function refreshMapping() {
  await sendCommand('get');
}

// Send updated mapping to device
async function applyMapping() {
  for (let i = 0; i < 7; i++) {
    const select = document.getElementById(`key-${i}`);
    let payload;
    if (select.value === '__random') {
      // ランダムオプション: 現在の保存配列を送信
      const arr = currentMapping[i] || [];
      const chars = arr.map(c => String.fromCharCode(c));
      payload = `${i} ${chars.join(' ')}`;
    } else {
      payload = `${i} ${select.value}`;
    }
    await sendCommand('setkey', payload);
  }
  await refreshMapping();
}

// Send a command over serial
async function sendCommand(cmd, payload = '') {
  const line = payload ? `${cmd}:${payload}\n` : `${cmd}\n`;
  await writer.write(new TextEncoder().encode(line));
}

// Initialize UI and event handlers
window.addEventListener('DOMContentLoaded', () => {
  for (let i = 0; i < 7; i++) {
    const select = document.getElementById(`key-${i}`);
    // Randomオプションを先頭に追加
    const randOpt = document.createElement('option');
    randOpt.textContent = 'Random';
    randOpt.value = '__random';
    select.appendChild(randOpt);
    // 単一キーオプションを追加
    keyOptions.forEach(opt => {
      const option = document.createElement('option');
      option.textContent = opt.label;
      option.value = opt.value;
      select.appendChild(option);
    });
  }
  document.getElementById('connect').addEventListener('click', connectDevice);
  document.getElementById('refresh').addEventListener('click', refreshMapping);
  document.getElementById('apply').addEventListener('click', applyMapping);
});
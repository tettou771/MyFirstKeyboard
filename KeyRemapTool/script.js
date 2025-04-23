const keyOptions = [
  { label: 'Enter', value: String.fromCharCode(0xB0) },
  { label: 'Space', value: ' ' },
  { label: 'Ctrl', value: String.fromCharCode(0xE0) },
  { label: 'Shift', value: String.fromCharCode(0xE1) },
  { label: 'Alt', value: String.fromCharCode(0xE2) },
  { label: 'GUI', value: String.fromCharCode(0xE3) },
];
// Add letters a-z (小文字のみ)
for (let c = 97; c <= 122; c++) {
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
  // エラー表示エリアをクリア
  const errDiv = document.getElementById('error');
  if (errDiv) errDiv.textContent = '';
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
    if (errDiv) errDiv.textContent = '接続エラー: ' + (err.message || err);
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
    // Arduino送信時に「:\」などでエスケープされているので解除
    // Arduino送信時に「\:」でエスケープされたコロンを解除
    let clean = payload.replace(/\\:/g, ':');
    let data;
    try {
      data = JSON.parse(clean);
    } catch (e) {
      console.error('Invalid JSON:', clean);
      return;
    }
    currentMapping = data;
    for (let i = 0; i < 7; i++) {
      const item = data[i];
      const select = document.getElementById(`key-${i}`);
      const strInput = document.getElementById(`str-${i}`);
      // 文字列マッピング対応
      if (item && typeof item.str === 'string') {
        select.value = '__string';
        strInput.value = item.str;
        strInput.style.display = 'block';
        continue;
      } else {
        strInput.style.display = 'none';
      }
      const arr = Array.isArray(item) ? item : [];
      const wrap = document.getElementById(`wrap-${i}`);
      if (arr.length > 1) {
        select.value = '__random';
      } else {
        const code = arr[0] || 0;
        if (code === 0) {
          // アップルマーク
          select.value = '__apple';
        } else {
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
  const applyBtn = document.getElementById('apply');
  applyBtn.disabled = true;
  // ランダムおよびアップルマーク用のコード配列
  const randomKeyCodes = [0x00, 0x21, 0x3F, 0x40, 0x23, 0x24, 0x25, 0x5E, 0x26, 0x2A, 0x2B, 0x2D, 0x7E];
  // 文字列モードのキーを保存
  const stringIndices = [];
  for (let i = 0; i < 7; i++) {
    const select = document.getElementById(`key-${i}`);
    const wrap = document.getElementById(`wrap-${i}`);
    // 文字列モードなら setstr を送信し、後で復元
    if (select.value === '__string') {
      const str = document.getElementById(`str-${i}`).value;
      const payload = `${i} ${str}`;
      await sendCommand('setstr', payload);
      stringIndices.push(i);
      continue;
    }
    let payload;
    if (select.value === '__random') {
      const hexs = randomKeyCodes.map(c => c.toString(16).padStart(2, '0').toUpperCase());
      payload = `${i} ${hexs.join(' ')}`;
    } else if (select.value === '__apple') {
      payload = `${i} 00`;
    } else {
      const code = select.value.charCodeAt(0);
      const hex = code.toString(16).padStart(2, '0').toUpperCase();
      payload = `${i} ${hex}`;
    }
    await sendCommand('setkey', payload);
  }
  // 常にリフレッシュして非文字列モードのキーだけ更新
  await refreshMapping();
  // 文字列モードのキーは再度Str選択と入力欄を表示
  for (const i of stringIndices) {
    const select = document.getElementById(`key-${i}`);
    select.value = '__string';
    const strInput = document.getElementById(`str-${i}`);
    strInput.style.display = 'block';
  }
  // フィードバック: 一時的にボタンを 'Applied!' に変更
  const originalText = applyBtn.textContent;
  applyBtn.textContent = 'Applied!';
  setTimeout(() => {
    applyBtn.textContent = originalText;
    applyBtn.disabled = false;
  }, 1000);
}

// Send a command over serial
async function sendCommand(cmd, payload = '') {
  const line = payload ? `${cmd}:${payload}\n` : `${cmd}\n`;
  console.log('[Serial Out]', line);
  await writer.write(new TextEncoder().encode(line));
}

// Initialize UI and event handlers
window.addEventListener('DOMContentLoaded', () => {
  // 定義済みのランダムキー候補 (アップルマークと記号)
  const randomKeyCodes = [0x00, 0x21, 0x3F, 0x40, 0x23, 0x24, 0x25, 0x5E, 0x26, 0x2A, 0x2B, 0x2D, 0x7E];
  for (let i = 0; i < 7; i++) {
    const select = document.getElementById(`key-${i}`);
    const wrap = document.getElementById(`wrap-${i}`);
    // 単一キーオプションを追加
    keyOptions.forEach(opt => {
      const option = document.createElement('option');
      option.textContent = opt.label;
      option.value = opt.value;
      select.appendChild(option);
    });
    // アップルマークオプションを追加
    const appleOpt = document.createElement('option');
    appleOpt.textContent = '';
    appleOpt.value = '__apple';
    select.appendChild(appleOpt);
    // ランダムオプションを追加
    const randOpt = document.createElement('option');
    randOpt.textContent = 'Rnd';
    randOpt.value = '__random';
    select.appendChild(randOpt);
    // 文字列オプションを追加
    const strOpt = document.createElement('option');
    strOpt.textContent = 'Str';
    strOpt.value = '__string';
    select.appendChild(strOpt);
    // 文字列入力欄を追加
    const strInput = document.createElement('input');
    strInput.type = 'text';
    strInput.id = `str-${i}`;
    strInput.placeholder = '文字列';
    strInput.style.display = 'none';
    wrap.appendChild(strInput);
    // 選択変更時の表示切替
    select.addEventListener('change', () => {
      if (select.value === '__string') strInput.style.display = 'block';
      else strInput.style.display = 'none';
    });
    // ↓ここから：読み込み時のデフォルト選択をセット
    if (i === 0) select.value = String.fromCharCode(0xB0);    // Enter (0xB0)
    else if (i === 1) select.value = ' '; // Space
    else if (i === 2) select.value = 'a';
    else if (i === 3) select.value = 'b';
    else if (i === 4) select.value = 'c';
    else if (i === 5) select.value = 'd';
    else if (i === 6) select.value = '__apple';
  }
  document.getElementById('connect').addEventListener('click', connectDevice);
  document.getElementById('refresh').addEventListener('click', refreshMapping);
  document.getElementById('apply').addEventListener('click', applyMapping);
});
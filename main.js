const { app, BrowserWindow, shell } = require('electron');
const URL = 'https://chispabot-fepo1.vercel.app/connect/';

function createWindow() {
  const win = new BrowserWindow({
    width: 520, height: 820, minWidth: 420,
    title: 'Chispa Connect', backgroundColor: '#0c0f1d'
  });
  const ses = win.webContents.session;
  // Permisos para hablar con el robot por USB y mandar avisos
  ses.setPermissionCheckHandler((wc, p) => ['serial', 'notifications'].includes(p));
  ses.setDevicePermissionHandler((d) => d.deviceType === 'serial');
  ses.setPermissionRequestHandler((wc, p, cb) => cb(['serial', 'notifications'].includes(p)));
  // Elige el primer puerto USB disponible
  ses.on('select-serial-port', (e, ports, wc, cb) => {
    e.preventDefault();
    cb(ports.length ? ports[0].portId : '');
  });
  win.webContents.setWindowOpenHandler(({ url }) => { shell.openExternal(url); return { action: 'deny' }; });
  win.webContents.on('did-fail-load', () => {
    win.loadURL('data:text/html;charset=utf-8,' + encodeURIComponent(
      '<body style="font-family:sans-serif;padding:24px"><h2>Sin conexión</h2><p>Chispa Connect necesita internet para abrir. Revisá tu conexión y reiniciá la app.</p></body>'));
  });
  win.loadURL(URL);
}

app.whenReady().then(createWindow);
app.on('window-all-closed', () => app.quit());

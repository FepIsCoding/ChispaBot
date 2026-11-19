const { app, BrowserWindow, shell } = require('electron');
const path = require('path');

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
  win.setMenuBarVisibility(false);
  win.loadFile(path.join(__dirname, 'app', 'index.html'));
}

app.whenReady().then(createWindow);
app.on('window-all-closed', () => app.quit());

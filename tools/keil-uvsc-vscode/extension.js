const vscode = require('vscode');
const path = require('path');
const fs = require('fs');

class KeilUvscFactory {
  createDebugAdapterDescriptor(session) {
    const configured = vscode.workspace.getConfiguration('robot2').get('pythonPath', 'py') || 'py';
    const python = configured;
    const sessionWorkspace = session.workspaceFolder?.fsPath;
    const configuredProject = session.configuration?.project;
    const projectWorkspace = configuredProject && !configuredProject.includes('${')
      ? path.resolve(path.dirname(path.dirname(configuredProject)))
      : undefined;
    const candidates = [
      projectWorkspace,
      sessionWorkspace,
      ...(vscode.workspace.workspaceFolders || []).map(folder => folder.uri.fsPath),
    ].filter(Boolean);
    const hasAdapter = folder => fs.existsSync(path.join(folder, 'tools', 'keil-uvsc', 'keil_uvsc_dap.py'));
    const workspace = candidates.find(hasAdapter)
      || candidates.map(folder => path.join(folder, 'Robot2')).find(hasAdapter)
      || candidates[0];
    const adapter = path.join(workspace, 'tools', 'keil-uvsc', 'keil_uvsc_dap.py');
    const args = path.basename(python).toLowerCase() === 'py.exe' || python === 'py'
      ? ['-3', adapter]
      : [adapter];
    return new vscode.DebugAdapterExecutable(python, args, { cwd: workspace });
  }
}

function activate(context) {
  context.subscriptions.push(
    vscode.debug.registerDebugAdapterDescriptorFactory('keil-uvsc', new KeilUvscFactory())
  );
}

function deactivate() {}

module.exports = { activate, deactivate };

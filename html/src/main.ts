import './styles.css';
import { Color } from 'three';
import { Assets } from './assets';
import { environments, flags } from './presets';
import type { EnvironmentId, FlagId } from './presets';
import { createScene } from './scene';
import { createViewer } from './viewer';
import type { Viewer } from './viewer';

function element<T extends HTMLElement>(id: string): T {
  const found = document.getElementById(id);
  if (!found) throw new Error(`Missing interface element: ${id}`);
  return found as T;
}

const environmentSelect = element<HTMLSelectElement>('environment');
const flagSelect = element<HTMLSelectElement>('flag');
const status = element<HTMLDivElement>('status');
const statusMessage = element<HTMLSpanElement>('status-message');
const retryButton = element<HTMLButtonElement>('retry');
const help = element<HTMLElement>('help');
const helpToggle = element<HTMLButtonElement>('help-toggle');
const label = element<HTMLSpanElement>('scene-label');
const assets = new Assets();
const app = element('app');
type Selection = 'environment' | 'flag';
const pending = new Map<Selection, string>();
const errors = new Map<Selection, { message: string; retry: () => void }>();
let viewer: Viewer | undefined;
let currentEnvironment: EnvironmentId = 'moon';
let currentFlag: FlagId = 'usa';
let environmentRequest = 0;
let flagRequest = 0;
let retry: (() => void) | undefined;
let ready = false;
let closed = false;
let graphicsMessage = '';

function showStatus(message: string, action?: () => void) {
  if (closed) return;
  status.hidden = !message;
  statusMessage.textContent = message;
  retry = action;
  retryButton.hidden = !action;
}

function updateStatus() {
  app.setAttribute('aria-busy', String(!ready || pending.size > 0));
  app.dataset.ready = String(ready && pending.size === 0);
  if (graphicsMessage) { showStatus(graphicsMessage); return; }
  if (!ready) return;
  const error = errors.values().next().value;
  if (error) showStatus(error.message, error.retry);
  else showStatus([...pending.values()].join(' · '));
}

function updateLabel() {
  label.textContent = `${environments[currentEnvironment].label} / ${flags[currentFlag].label}`;
}

function toggleHelp(open = help.hidden) {
  help.hidden = !open;
  helpToggle.setAttribute('aria-expanded', String(open));
}

environmentSelect.disabled = true;
flagSelect.disabled = true;
helpToggle.addEventListener('click', () => toggleHelp());
element('help-close').addEventListener('click', () => { toggleHelp(false); helpToggle.focus(); });
element('reset').addEventListener('click', () => { viewer?.reset(); viewer?.canvas.focus(); });
retryButton.addEventListener('click', () => retry?.());
document.addEventListener('keydown', (event) => {
  if (event.key === 'Escape') { toggleHelp(false); return; }
  if ((event.target === viewer?.canvas || event.target === document.body) && event.key.toLowerCase() === 'h') {
    event.preventDefault();
    toggleHelp();
  }
});

async function start() {
  showStatus('Preparing the lunar module…');
  try {
    const world = createScene();
    viewer = createViewer(element('viewport'), world.scene, world.assembly,
      (message) => { graphicsMessage = message; updateStatus(); }, world.inspectionLights);
    viewer.canvas.style.visibility = 'hidden';

    async function changeEnvironment(id: EnvironmentId) {
      const request = ++environmentRequest;
      pending.set('environment', `Loading ${environments[id].label} terrain…`);
      errors.delete('environment');
      updateStatus();
      try {
        const texture = await assets.load(environments[id].texture);
        if (closed || request !== environmentRequest) return;
        world.materials.ground.map = texture;
        world.materials.ground.needsUpdate = true;
        world.scene.background = new Color(environments[id].background);
        currentEnvironment = id;
        environmentSelect.value = id;
        updateLabel();
        viewer?.render();
      } catch {
        if (closed || request !== environmentRequest) return;
        environmentSelect.value = currentEnvironment;
        errors.set('environment', {
          message: `Could not load ${environments[id].label}. Your previous location is still active.`,
          retry: () => { void changeEnvironment(id); },
        });
      } finally {
        if (request === environmentRequest) pending.delete('environment');
        updateStatus();
      }
    }

    async function changeFlag(id: FlagId) {
      const request = ++flagRequest;
      pending.set('flag', `Loading ${flags[id].label} flag…`);
      errors.delete('flag');
      updateStatus();
      try {
        const texture = await assets.load(flags[id].texture);
        if (closed || request !== flagRequest) return;
        world.materials.flag.map = texture;
        world.materials.flag.needsUpdate = true;
        currentFlag = id;
        flagSelect.value = id;
        updateLabel();
        viewer?.render();
      } catch {
        if (closed || request !== flagRequest) return;
        flagSelect.value = currentFlag;
        errors.set('flag', {
          message: `Could not load the ${flags[id].label} flag. Your previous flag is still active.`,
          retry: () => { void changeFlag(id); },
        });
      } finally {
        if (request === flagRequest) pending.delete('flag');
        updateStatus();
      }
    }

    const [ground, flag] = await Promise.all([assets.load('moon'), assets.load('usa')]);
    if (closed) return;
    world.materials.ground.map = ground;
    world.materials.flag.map = flag;
    world.materials.ground.needsUpdate = true;
    world.materials.flag.needsUpdate = true;
    viewer.canvas.style.visibility = '';
    viewer.render();
    ready = true;
    environmentSelect.disabled = false;
    flagSelect.disabled = false;
    updateStatus();
    environmentSelect.addEventListener('change', () => { void changeEnvironment(environmentSelect.value as EnvironmentId); });
    flagSelect.addEventListener('change', () => { void changeFlag(flagSelect.value as FlagId); });
  } catch (error) {
    viewer?.dispose();
    viewer = undefined;
    const message = error instanceof Error ? error.message : '';
    const graphicsError = /WebGL|context|Error creating/i.test(message);
    showStatus(graphicsError
      ? 'WebGL 2 is unavailable. Enable graphics acceleration or open the viewer in a supported browser.'
      : 'Could not load the scene. Check the connection and retry.', () => { void start(); });
  }
}

window.addEventListener('pagehide', (event) => {
  if (event.persisted) return;
  closed = true;
  viewer?.dispose();
  assets.dispose();
});

updateStatus();
void start();

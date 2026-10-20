// Flatten the native cabin into browser mesh data; not required to run the viewer.
import { readFile, writeFile, mkdir } from 'node:fs/promises';
import { fileURLToPath } from 'node:url';
import { Matrix4, Vector3 } from 'three';

const root = new URL('../../', import.meta.url);
const output = new URL('../src/cabin-data.json', import.meta.url);
const source = await readFile(new URL('src/scene.c', root), 'utf8');
const bodies = new Map([...source.matchAll(
  /static void (cabin(?:Shell|Tower|DetailTop|DetailLower)\d+)\(void\) \{([\s\S]*?)\n\}/g,
)].map((match) => [match[1], match[2]]));
const number = (value) => Number.parseFloat(value);
const rounded = (value) => Math.round(value * 1e6) / 1e6;

function extract(name) {
  const panels = [];
  const cylinders = [];
  function walk(functionName, initial) {
    const body = bodies.get(functionName);
    if (!body) throw new Error(`Missing native geometry: ${functionName}`);
    let matrix = initial.clone();
    const stack = [];
    const operations = /DRAW_CABIN_PANEL\(([\s\S]*?)\);|\b(glPushMatrix|glPopMatrix|glTranslatef|glRotatef|glScalef|cabin(?:Shell|Tower|DetailTop|DetailLower)\d+|drawSolidCylinder)\(([^;]*?)\);/g;
    for (const match of body.matchAll(operations)) {
      if (match[1]) {
        panels.push([...match[1].matchAll(/\{([^}]+)\}/g)].map((vertex) => {
          const [x, y, z] = vertex[1].split(',').map(number);
          return new Vector3(x, y, z).applyMatrix4(matrix).toArray().map(rounded);
        }));
        continue;
      }
      const operation = match[2];
      const args = match[3].split(',').map(number);
      if (operation === 'glPushMatrix') stack.push(matrix.clone());
      else if (operation === 'glPopMatrix') {
        const saved = stack.pop();
        if (!saved) throw new Error(`Unbalanced matrix stack in ${functionName}`);
        matrix = saved;
      } else if (operation === 'glTranslatef') matrix.multiply(new Matrix4().makeTranslation(...args));
      else if (operation === 'glScalef') matrix.multiply(new Matrix4().makeScale(...args));
      else if (operation === 'glRotatef') {
        matrix.multiply(new Matrix4().makeRotationAxis(new Vector3(...args.slice(1)).normalize(), args[0] * Math.PI / 180));
      } else if (operation === 'drawSolidCylinder') cylinders.push(matrix.toArray().map(rounded));
      else walk(operation, matrix);
    }
    if (stack.length) throw new Error(`Unbalanced matrix stack in ${functionName}`);
  }
  walk(name, new Matrix4());
  orientPanels(panels);
  return { panels, cylinders };
}

// Share edge orientation even where one panel ends halfway along another edge.
// This gives solids outward-facing triangles instead of relying on double-sided materials.
function orientPanels(panels) {
  const key = (point) => point.join(',');
  const vertices = [...new Map(panels.flat().map((p) => [key(p), new Vector3(...p)])).values()];
  const edges = new Map();
  panels.forEach((panel, face) => panel.forEach((point, index) => {
    const a = new Vector3(...point);
    const b = new Vector3(...panel[(index + 1) % panel.length]);
    const delta = b.clone().sub(a);
    const length = delta.lengthSq();
    if (length < 1e-12) return;
    const splits = vertices.map((p) => ({ p, t: p.clone().sub(a).dot(delta) / length }))
      .filter(({ p, t }) => t >= -1e-6 && t <= 1 + 1e-6 && a.clone().addScaledVector(delta, t).distanceToSquared(p) < 1e-10)
      .sort((x, y) => x.t - y.t);
    for (let i = 0; i + 1 < splits.length; i++) {
      const start = key(splits[i].p.toArray());
      const end = key(splits[i + 1].p.toArray());
      const edgeKey = [start, end].sort().join('|');
      if (!edges.has(edgeKey)) edges.set(edgeKey, []);
      edges.get(edgeKey).push({ face, direction: start < end });
    }
  }));
  const neighbors = panels.map(() => []);
  for (const edge of edges.values()) {
    if (edge.length !== 2) continue; // Intersecting attachment joins are not manifold edges.
    const [a, b] = edge;
    const flip = a.direction === b.direction;
    neighbors[a.face].push({ face: b.face, flip });
    neighbors[b.face].push({ face: a.face, flip });
  }
  const flips = new Map();
  panels.forEach((_, seed) => {
    if (flips.has(seed)) return;
    const component = [seed];
    flips.set(seed, false);
    for (let i = 0; i < component.length; i++) {
      const face = component[i];
      for (const next of neighbors[face]) {
        if (flips.has(next.face)) continue;
        flips.set(next.face, flips.get(face) !== next.flip);
        component.push(next.face);
      }
    }
    let volume = 0;
    for (const face of component) {
      const points = panels[face].map((p) => new Vector3(...p));
      for (let i = 1; i + 1 < points.length; i++) {
        volume += points[0].dot(points[i].clone().cross(points[i + 1])) * (flips.get(face) ? -1 : 1);
      }
    }
    for (const face of component) if (flips.get(face) !== (volume < 0)) panels[face].reverse();
  });
}

const data = {
  shell: extract('cabinTower15'),
  upper: extract('cabinDetailTop12'),
  lower: extract('cabinDetailLower12'),
};
await mkdir(new URL('../src/', import.meta.url), { recursive: true });
const parts = Object.entries(data).map(([name, part]) =>
  `  "${name}": {\n    "panels": [\n${part.panels.map((p) => `      ${JSON.stringify(p)}`).join(',\n')}\n    ],\n    "cylinders": ${JSON.stringify(part.cylinders)}\n  }`);
await writeFile(output, `{\n${parts.join(',\n')}\n}\n`);

console.log(`Imported ${Object.values(data).reduce((n, part) => n + part.panels.length, 0)} native panels into ${fileURLToPath(output)}`);

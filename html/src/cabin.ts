import { BufferGeometry, Float32BufferAttribute, LineBasicMaterial, LineSegments, Matrix4 } from 'three';
import type { Material, Object3D } from 'three';
import data from './cabin-data.json';
import { panelGeometry, part, rotate, solid, translate } from './geometry';

export function createCabin(parent: Object3D, material: Material, cylinder: BufferGeometry) {
  const edgeMaterial = new LineBasicMaterial({ color: 0x202630, transparent: true, opacity: 0.58 });
  function attachment(group: Object3D, definition: { panels: number[][][]; cylinders: number[][] }, name: string) {
    const geometry = panelGeometry(definition.panels);
    const mesh = solid(group, geometry, material, name);
    // Offset only the fill so the original panel outlines remain legible.
    mesh.material = material.clone();
    mesh.material.polygonOffset = true;
    mesh.material.polygonOffsetFactor = 1;
    mesh.material.polygonOffsetUnits = 1;
    const lines: number[] = [];
    for (const panel of definition.panels) for (let i = 0; i < panel.length; i++) {
      lines.push(...panel[i], ...panel[(i + 1) % panel.length]);
    }
    const edges = new BufferGeometry();
    edges.setAttribute('position', new Float32BufferAttribute(lines, 3));
    const outline = new LineSegments(edges, edgeMaterial);
    outline.name = `${name} panel boundaries`;
    group.add(outline);
    definition.cylinders.forEach((matrix) => {
      part(group, 'Attachment cylinder', [new Matrix4().fromArray(matrix)],
        (local) => solid(local, cylinder, material, 'Cabin cylinder'));
    });
  }
  attachment(parent, data.shell, 'Cabin shell');
  part(parent, 'Upper attachment', [rotate(90, 'y'), translate(0.65, 0.1, 0)],
    (group) => attachment(group, data.upper, 'Upper attachment'));
  part(parent, 'Lower attachment', [rotate(270, 'y'), translate(-0.975, 0.7, 1)],
    (group) => attachment(group, data.lower, 'Lower attachment'));
}

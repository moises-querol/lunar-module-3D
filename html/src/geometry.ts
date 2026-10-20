import { BufferGeometry, Float32BufferAttribute, Group, Matrix4, Mesh, Vector3 } from 'three';
import type { Material, Object3D } from 'three';

type Point = readonly number[];
export const translate = (x: number, y: number, z: number) => new Matrix4().makeTranslation(x, y, z);
export const scale = (x: number, y: number, z: number) => new Matrix4().makeScale(x, y, z);
export const rotate = (angle: number, axis: 'x' | 'y' | 'z') => new Matrix4().makeRotationAxis(
  new Vector3(axis === 'x' ? 1 : 0, axis === 'y' ? 1 : 0, axis === 'z' ? 1 : 0), angle * Math.PI / 180,
);

// Preserve the native transform order, including translations after scaling.
export function part(parent: Object3D, name: string, transforms: Matrix4[], build?: (group: Group) => void) {
  const group = new Group();
  group.name = name;
  group.matrixAutoUpdate = false;
  for (const matrix of transforms) group.matrix.multiply(matrix);
  parent.add(group);
  build?.(group);
  return group;
}

export function panelGeometry(panels: readonly (readonly Point[])[]) {
  const positions: number[] = [];
  for (const points of panels) {
    for (let i = 1; i + 1 < points.length; i++) {
      const a = new Vector3(...points[0]);
      const b = new Vector3(...points[i]);
      const c = new Vector3(...points[i + 1]);
      if (b.clone().sub(a).cross(c.clone().sub(a)).lengthSq() < 1e-16) continue;
      positions.push(...points[0], ...points[i], ...points[i + 1]);
    }
  }
  const geometry = new BufferGeometry();
  geometry.setAttribute('position', new Float32BufferAttribute(positions, 3));
  geometry.computeVertexNormals();
  geometry.computeBoundingSphere();
  return geometry;
}

export function solid(parent: Object3D, geometry: BufferGeometry, material: Material, name: string) {
  const mesh = new Mesh(geometry, material);
  mesh.name = name;
  mesh.castShadow = true;
  mesh.receiveShadow = true;
  parent.add(mesh);
  return mesh;
}

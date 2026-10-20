import { Color, DirectionalLight, Float32BufferAttribute, Group, HemisphereLight, Scene } from 'three';
import { createMaterials } from './materials';
import { createModel } from './model';
import { panelGeometry, part, scale, solid, translate } from './geometry';
import { CylinderGeometry } from 'three';

export function createScene() {
  const scene = new Scene();
  scene.background = new Color('#000000');
  const materials = createMaterials();
  const assembly = new Group();
  assembly.name = 'Original model and flag';
  // Lift the native scene's ground plane to Y=0 without changing relative geometry.
  assembly.position.y = 0.75;
  assembly.add(createModel(materials));
  scene.add(assembly);

  const ground = solid(scene, panelGeometry([
    [[-2.5, 0, 2.5], [2.5, 0, 2.5], [2.5, 0, -2.5], [-2.5, 0, -2.5]],
  ]), materials.ground, 'Terrain');
  // Match the native ground quad's rotated XY coordinates and UV mapping.
  ground.geometry.setAttribute('uv', new Float32BufferAttribute([0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0], 2));
  const flag = solid(assembly, panelGeometry([
    [[1.75, 0.25, 1.25], [2.25, 0.25, 1.25], [2.25, -0.15, 1.25], [1.75, -0.15, 1.25]],
  ]), materials.flag, 'Flag cloth');
  flag.geometry.setAttribute('uv', new Float32BufferAttribute([0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1], 2));
  const pole = new CylinderGeometry(0.5, 0.5, 1, 32);
  part(assembly, 'Flagpole', [translate(1.75, -0.25, 1.25), scale(0.03, 1, 0.03)],
    (group) => solid(group, pole, materials.aluminum, 'Flagpole'));

  const key = new DirectionalLight(0xffedd3, 2.3);
  key.name = 'Key light';
  key.position.set(-3, 5, 4);
  key.castShadow = true;
  key.shadow.mapSize.set(2048, 2048);
  key.shadow.camera.left = -3.5;
  key.shadow.camera.right = 3.5;
  key.shadow.camera.top = 3.5;
  key.shadow.camera.bottom = -3.5;
  key.shadow.camera.near = 0.5;
  key.shadow.camera.far = 15;
  key.shadow.normalBias = 0.025;
  key.shadow.bias = -0.0002;
  key.target.position.set(0, 0.7, 0);
  const fill = new DirectionalLight(0xadbfe0, 0.7);
  fill.position.set(4, 2, 1);
  const rim = new DirectionalLight(0xd2dcff, 0.85);
  rim.position.set(0, 3, -4);
  scene.add(key, key.target, fill, fill.target, rim, rim.target, new HemisphereLight(0xdbe3f0, 0x37312a, 0.2));

  return { scene, assembly, materials, ground, flag, inspectionLights: [key, fill, rim] };
}

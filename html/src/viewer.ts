import {
  ACESFilmicToneMapping, Box3, MathUtils, Mesh, PCFSoftShadowMap,
  PerspectiveCamera, PMREMGenerator, Spherical, SRGBColorSpace, Vector3, WebGLRenderer,
} from 'three';
import type { BufferGeometry, DirectionalLight, Material, Object3D, Scene } from 'three';
import { OrbitControls } from 'three/addons/controls/OrbitControls.js';
import { RoomEnvironment } from 'three/addons/environments/RoomEnvironment.js';

export function createViewer(container: HTMLElement, scene: Scene, assembly: Object3D, status: (message: string) => void, lights: DirectionalLight[]) {
  const renderer = new WebGLRenderer({ antialias: true, powerPreference: 'high-performance' });
  renderer.outputColorSpace = SRGBColorSpace;
  renderer.toneMapping = ACESFilmicToneMapping;
  renderer.toneMappingExposure = 1;
  renderer.shadowMap.enabled = true;
  renderer.shadowMap.type = PCFSoftShadowMap;
  const canvas = renderer.domElement;
  canvas.tabIndex = 0;
  canvas.setAttribute('aria-label', 'Interactive lunar module. Drag to orbit, scroll to zoom, right-drag to pan. Press H for all controls.');
  canvas.setAttribute('aria-describedby', 'scene-description');
  container.append(canvas);

  function createEnvironment() {
    const generator = new PMREMGenerator(renderer);
    const room = new RoomEnvironment();
    const environment = generator.fromScene(room, 0.04);
    scene.environment = environment.texture;
    scene.environmentIntensity = 0.35;
    room.dispose();
    generator.dispose();
    return environment;
  }
  let environment = createEnvironment();
  const lightDirections = lights.map((light) => light.position.clone());

  const camera = new PerspectiveCamera(50, 1, 0.05, 50);
  const controls = new OrbitControls(camera, canvas);
  controls.enableDamping = true;
  controls.dampingFactor = 0.12;
  controls.minDistance = 1.4;
  controls.maxDistance = 15;
  controls.maxPolarAngle = Math.PI * 0.495;
  controls.screenSpacePanning = true;
  const bounds = new Box3().setFromObject(assembly);
  const center = bounds.getCenter(new Vector3());
  controls.cursor.copy(center);
  controls.maxTargetRadius = 2.5;
  const direction = new Vector3(-3.6, 4.4, 4.8).normalize();
  const points: Vector3[] = [];
  assembly.updateWorldMatrix(true, true);
  assembly.traverse((object) => {
    if (!(object instanceof Mesh)) return;
    const positions = object.geometry.getAttribute('position');
    for (let i = 0; i < positions.count; i++) {
      points.push(new Vector3().fromBufferAttribute(positions, i).applyMatrix4(object.matrixWorld).sub(center));
    }
  });
  let frame = 0;
  let disposed = false;
  let initialView = true;
  let contextLost = false;

  function requestRender() {
    if (!disposed && !frame && !document.hidden && !contextLost) frame = requestAnimationFrame(render);
  }
  function render() {
    frame = 0;
    if (disposed || document.hidden || contextLost) return;
    const changed = controls.update();
    // Match the original eye-space lighting as the camera orbits the model.
    lights.forEach((light, index) => {
      light.target.position.copy(controls.target);
      light.position.copy(lightDirections[index]).applyQuaternion(camera.quaternion).add(controls.target);
      light.target.updateMatrixWorld();
    });
    renderer.render(scene, camera);
    if (changed) requestRender();
  }

  function reset() {
    // Clear pending damping deltas before fitting the original scene again.
    controls.enableDamping = false;
    controls.update();
    controls.target.copy(center);
    camera.position.copy(center).add(direction);
    camera.lookAt(center);
    camera.updateMatrixWorld();
    const right = new Vector3().setFromMatrixColumn(camera.matrixWorld, 0);
    const up = new Vector3().setFromMatrixColumn(camera.matrixWorld, 1);
    const tanV = Math.tan(MathUtils.degToRad(camera.fov / 2));
    const tanH = tanV * camera.aspect;
    let distance = 0;
    for (const corner of points) {
      distance = Math.max(distance, corner.dot(direction) + Math.max(Math.abs(corner.dot(right)) / tanH, Math.abs(corner.dot(up)) / tanV));
    }
    camera.position.copy(center).addScaledVector(direction, distance * 1.13);
    controls.update();
    controls.enableDamping = true;
    initialView = true;
    requestRender();
  }

  function resize() {
    const width = Math.max(1, container.clientWidth);
    const height = Math.max(1, container.clientHeight);
    renderer.setPixelRatio(Math.min(window.devicePixelRatio || 1, 2));
    renderer.setSize(width, height);
    camera.aspect = width / height;
    camera.updateProjectionMatrix();
    if (initialView) reset();
    requestRender();
  }

  function onKeyboard(event: KeyboardEvent) {
    if (event.ctrlKey || event.metaKey || event.altKey) return;
    const arrows = ['ArrowLeft', 'ArrowRight', 'ArrowUp', 'ArrowDown'];
    if (event.key.toLowerCase() === 'r') {
      event.preventDefault();
      reset();
      return;
    }
    if (!arrows.includes(event.key) && !['+', '=', '-', '_'].includes(event.key)) return;
    event.preventDefault();
    initialView = false;
    const offset = camera.position.clone().sub(controls.target);
    if (event.shiftKey && arrows.includes(event.key)) {
      const right = new Vector3().setFromMatrixColumn(camera.matrixWorld, 0);
      const up = new Vector3().setFromMatrixColumn(camera.matrixWorld, 1);
      const delta = offset.length() * 0.025;
      const movement = event.key === 'ArrowLeft' ? right.multiplyScalar(-delta)
        : event.key === 'ArrowRight' ? right.multiplyScalar(delta)
        : event.key === 'ArrowUp' ? up.multiplyScalar(delta) : up.multiplyScalar(-delta);
      camera.position.add(movement);
      controls.target.add(movement);
    } else {
      const spherical = new Spherical().setFromVector3(offset);
      if (event.key === 'ArrowLeft') spherical.theta -= Math.PI / 24;
      if (event.key === 'ArrowRight') spherical.theta += Math.PI / 24;
      if (event.key === 'ArrowUp') spherical.phi -= Math.PI / 24;
      if (event.key === 'ArrowDown') spherical.phi += Math.PI / 24;
      if (event.key === '+' || event.key === '=') spherical.radius *= 0.9;
      if (event.key === '-' || event.key === '_') spherical.radius *= 1.1;
      spherical.phi = MathUtils.clamp(spherical.phi, 0.05, controls.maxPolarAngle);
      spherical.radius = MathUtils.clamp(spherical.radius, controls.minDistance, controls.maxDistance);
      camera.position.copy(controls.target).add(new Vector3().setFromSpherical(spherical));
    }
    controls.update();
    requestRender();
  }

  function onStart() { initialView = false; }
  function focusCanvas() { canvas.focus({ preventScroll: true }); }
  function onVisibility() {
    if (document.hidden) {
      cancelAnimationFrame(frame);
      frame = 0;
    } else requestRender();
  }
  function onContextLost(event: Event) {
    event.preventDefault();
    contextLost = true;
    status('Graphics connection interrupted. Waiting to restore the scene…');
  }
  function onContextRestored() {
    environment.dispose();
    environment = createEnvironment();
    renderer.shadowMap.needsUpdate = true;
    contextLost = false;
    status('');
    requestRender();
  }
  controls.addEventListener('change', requestRender);
  controls.addEventListener('start', onStart);
  canvas.addEventListener('keydown', onKeyboard);
  canvas.addEventListener('pointerdown', focusCanvas);
  canvas.addEventListener('webglcontextlost', onContextLost);
  canvas.addEventListener('webglcontextrestored', onContextRestored);
  document.addEventListener('visibilitychange', onVisibility);
  const observer = new ResizeObserver(resize);
  observer.observe(container);
  resize();

  function dispose() {
    disposed = true;
    cancelAnimationFrame(frame);
    observer.disconnect();
    controls.dispose();
    controls.removeEventListener('change', requestRender);
    controls.removeEventListener('start', onStart);
    canvas.removeEventListener('keydown', onKeyboard);
    canvas.removeEventListener('pointerdown', focusCanvas);
    canvas.removeEventListener('webglcontextlost', onContextLost);
    canvas.removeEventListener('webglcontextrestored', onContextRestored);
    document.removeEventListener('visibilitychange', onVisibility);
    const geometries = new Set<BufferGeometry>();
    const materials = new Set<Material>();
    scene.traverse((object) => {
      if (object instanceof Mesh || 'geometry' in object) {
        const renderable = object as Mesh;
        geometries.add(renderable.geometry);
        for (const material of Array.isArray(renderable.material) ? renderable.material : [renderable.material]) materials.add(material);
      }
    });
    for (const geometry of geometries) geometry.dispose();
    for (const material of materials) material.dispose();
    environment.dispose();
    for (const light of lights) light.shadow.dispose();
    renderer.dispose();
    canvas.remove();
  }

  return { render: requestRender, reset, dispose, canvas };
}

export type Viewer = ReturnType<typeof createViewer>;

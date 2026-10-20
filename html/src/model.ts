import { ConeGeometry, CylinderGeometry, Group, SphereGeometry } from 'three';
import type { Object3D } from 'three';
import { createCabin } from './cabin';
import { panelGeometry, part, rotate, scale, solid, translate } from './geometry';
import type { Materials } from './materials';

function baseGeometry() {
  const radii = [0.47, 0.5, 0.5, 0.47];
  const heights = [-0.25, -0.22, 0.22, 0.25];
  const ring = (band: number, side: number) => [
    radii[band] * Math.cos(side * Math.PI / 4), heights[band], radii[band] * Math.sin(side * Math.PI / 4),
  ];
  const panels: number[][][] = [];
  for (let band = 0; band < 3; band++) for (let side = 0; side < 8; side++) {
    panels.push([ring(band, side), ring(band + 1, side), ring(band + 1, side + 1), ring(band, side + 1)]);
  }
  panels.push(Array.from({ length: 8 }, (_, i) => ring(0, i)));
  panels.push(Array.from({ length: 8 }, (_, i) => ring(3, 7 - i)));
  return panelGeometry(panels);
}

export function createModel(materials: Materials) {
  const model = new Group();
  model.name = 'Lunar module';
  const cylinder = new CylinderGeometry(0.5, 0.5, 1, 50, 1);
  const sphere = new SphereGeometry(0.5, 32, 24);
  // The original cone grows from the lower tip to the upper rim.
  const cone = new ConeGeometry(0.5, 1, 50).rotateZ(Math.PI);
  const drawCylinder = (parent: Object3D, material = materials.copper) => solid(parent, cylinder, material, 'Cylinder');

  function drawPole(parent: Object3D) {
    part(parent, 'Strut', [scale(0.1, 1.5, 0.1)], drawCylinder);
    part(parent, 'Upper strut end', [scale(0.1, 0.1, 0.1), rotate(180, 'x'), translate(0, -8, 0)],
      (group) => solid(group, cone, materials.copper, 'Taper'));
    part(parent, 'Lower strut end', [scale(0.1, 0.1, 0.1), translate(0, -8, 0)],
      (group) => solid(group, cone, materials.copper, 'Taper'));
  }

  function drawLadder(parent: Object3D) {
    part(parent, 'Ladder rails', [scale(0.05, 1.2, 0.05)], (group) => {
      drawCylinder(group);
      part(group, 'Second rail', [translate(8, 0, 0)], drawCylinder);
    });
    for (const height of [0, 0.18, 0.36, 0.54, -0.18, -0.36, -0.54]) {
      part(parent, 'Ladder rung', [rotate(90, 'z'), translate(height, -0.2, 0), scale(0.04, 0.4, 0.04)], drawCylinder);
    }
  }

  part(model, 'Descent base', [translate(0, 0.03, 0)], (base) => {
    part(base, 'Beveled gold body', [rotate(112.5, 'y'), translate(0, -0.04, 0), scale(1.75, 1.2, 1.75)],
      (group) => solid(group, baseGeometry(), materials.gold, 'Octagonal base'));
    part(base, 'Underside cylinder', [translate(0, -0.2, 0), scale(1, 0.5, 1)],
      (group) => drawCylinder(group, materials.gold));
    part(base, 'Side fitting', [translate(0.4, -0.15, 0.7), scale(0.1, 0.3, 0.1)],
      (group) => drawCylinder(group, materials.gold));

    part(base, 'Cabin placement', [rotate(90, 'y'), translate(0.3, 0.325, 0.8)], (group) => createCabin(group, materials.aluminum, cylinder));

    [0, 180, 270, 90].forEach((angle, index) => {
      part(base, `Landing leg ${index + 1}`, [rotate(angle, 'y'), translate(0, 0.2, -1.2), scale(0.65, 0.65, 0.65)], (leg) => {
        part(leg, 'Main leg', [rotate(25, 'x'), translate(0, -0.85, 0)], (group) => {
          drawPole(group);
          if (index === 1) part(group, 'Access ladder', [translate(-0.2, 0, -0.05)], drawLadder);
        });
        for (const braceAngle of [35, -35]) {
          part(leg, 'Horizontal brace', [rotate(braceAngle, 'y'), rotate(90, 'x'), translate(0, 0.4, 0), scale(1, 0.5, 1)], drawPole);
        }
        part(leg, 'Diagonal brace', [rotate(-47.5, 'x'), translate(0, -0.5, 0), scale(1, 0.6, 1)], drawPole);
        part(leg, 'Footpad', [translate(0, -1.48, -0.7), scale(0.6, 0.04, 0.6)], drawCylinder);
        part(leg, 'Leg joint', [scale(0.2, 0.2, 0.2)], (group) => solid(group, sphere, materials.copper, 'Joint'));
      });
    });

    part(base, 'Access ramp', [translate(-0.175, 0.275, 1.2)], (ramp) => {
      solid(ramp, panelGeometry([[[0, 0, 0], [0.35, 0, 0], [0.35, 0, -0.8], [0, 0, -0.8]]]), materials.copper, 'Ramp deck');
      for (const x of [0.015, 0.335]) {
        part(ramp, 'Railing post', [translate(x, 0.1, -0.04), scale(0.02, 0.2, 0.02)], drawCylinder);
        part(ramp, 'Railing', [rotate(90, 'x'), translate(x, -0.235, -0.195), scale(0.02, 0.4, 0.02)], drawCylinder);
      }
    });
  });
  return model;
}

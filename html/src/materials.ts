import { Color, DoubleSide, MeshStandardMaterial, SRGBColorSpace } from 'three';

export function createMaterials() {
  // Keep the native palette; rough metals make the original facets readable.
  const color = (r: number, g: number, b: number) => new Color().setRGB(r, g, b, SRGBColorSpace);
  return {
    gold: new MeshStandardMaterial({ color: color(0.74, 0.53, 0.16), metalness: 0.65, roughness: 0.48 }),
    aluminum: new MeshStandardMaterial({ color: color(0.62, 0.65, 0.70), metalness: 0.42, roughness: 0.57 }),
    copper: new MeshStandardMaterial({ color: color(0.65, 0.34, 0.18), metalness: 0.55, roughness: 0.43 }),
    ground: new MeshStandardMaterial({ color: 0xd8d8d8, roughness: 1, metalness: 0 }),
    flag: new MeshStandardMaterial({ color: 0xffffff, roughness: 1, metalness: 0, side: DoubleSide }),
  };
}

export type Materials = ReturnType<typeof createMaterials>;

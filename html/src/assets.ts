import { LinearFilter, LinearMipmapLinearFilter, SRGBColorSpace } from 'three';
import { TGALoader } from 'three/addons/loaders/TGALoader.js';
import type { Texture } from 'three';
import type { TextureId } from './presets';
import moon from '../../assets/textures/moon.tga?url';
import mars from '../../assets/textures/mars.tga?url';
import venus from '../../assets/textures/venus.tga?url';
import usa from '../../assets/textures/usa.tga?url';
import spain from '../../assets/textures/spain.tga?url';
import ussr from '../../assets/textures/ussr.tga?url';

const textureUrls: Record<TextureId, string> = { moon, mars, venus, usa, spain, ussr };

export class Assets {
  private readonly loader = new TGALoader();
  private readonly textures = new Map<TextureId, Promise<Texture>>();
  private disposed = false;

  load(id: TextureId): Promise<Texture> {
    const existing = this.textures.get(id);
    if (existing) return existing;
    const pending = this.loader.loadAsync(textureUrls[id])
      .then((texture) => {
        if (this.disposed) {
          texture.dispose();
          throw new Error('Viewer has been closed.');
        }
        texture.colorSpace = SRGBColorSpace;
        // Keep TGALoader's top-to-bottom rows and mirror X to reproduce the
        // native uploader's reversed pixel order without duplicating the asset.
        texture.flipY = false;
        texture.repeat.x = -1;
        texture.offset.x = 1;
        texture.magFilter = LinearFilter;
        texture.minFilter = LinearMipmapLinearFilter;
        texture.anisotropy = 4;
        return texture;
      })
      .catch((error: unknown) => {
        this.textures.delete(id);
        throw error;
      });
    this.textures.set(id, pending);
    return pending;
  }

  dispose() {
    this.disposed = true;
    for (const pending of this.textures.values()) void pending.then((texture) => texture.dispose()).catch(() => {});
    this.textures.clear();
  }
}

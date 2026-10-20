export const environments = {
  moon: { label: 'Moon', background: '#000000', texture: 'moon' },
  mars: { label: 'Mars', background: '#b36e36', texture: 'mars' },
  venus: { label: 'Venus', background: '#edc482', texture: 'venus' },
} as const;

export const flags = {
  usa: { label: 'USA', texture: 'usa' },
  spain: { label: 'Spain', texture: 'spain' },
  ussr: { label: 'USSR', texture: 'ussr' },
} as const;

export type EnvironmentId = keyof typeof environments;
export type FlagId = keyof typeof flags;
export type TextureId = EnvironmentId | FlagId;

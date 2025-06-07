# @title Weird Black and White Texture
# @tags render

filter org.mathmap.wbawt (float granularity: 0-50 (10),
              float z1: 0-10 (0), float z2: 0-10 (2),
              float w2: 0-1 (0.5), float f2: 1-10 (4),
              float threshold: -1-1 (-0.3),
              float contrast: 0-10 (1.5))
"Just something I played around with when thinking about automatically creating levels for a Worms-like game."
    nxy = xy*granularity;
    n=noise([nxy[0]/2,    nxy[1],    z1]) +
      noise([nxy[0]/2*f2, nxy[1]*f2, z2]) * w2;
    grayColor(clamp((n-threshold) * contrast, 0, 1))
end

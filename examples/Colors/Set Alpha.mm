# @title Set Alpha
# @tags colors

filter org.mathmap.color_set_alpha (image in, image mask)
  p = in(xy);
  m = mask(xy);
  rgba:[red(p), green(p), blue(p), gray(m)]
end
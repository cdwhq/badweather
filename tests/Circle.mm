filter circle (float radius)
  grayColor(if r < radius then t else 0 end)
end

filter bla (image in, float radius: 0-1.5 (1))
  in(xy) * circle(radius, xy, 1-t)
end

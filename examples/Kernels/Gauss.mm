# @title Gauss
# @tags kernels

filter org.mathmap.kernel_gauss (float phi: 0.00001-1)
  g = exp(-r*r/(2*phi*phi));
  rgba:[g, g, g, g]
end

# @title Radial Displace
# @tags map

# tomr@aceldama.com
filter org.mathmap.radial_displace (image in, image radial, float max_r: 0-1 (0.2),
                        image angle, float max_a: 0-6.2831853 (0.5))
"This is a displace filter which works on the polar instead of the cartesian coordinate system."
    in(ra:[r+t*max_r*((2*gray(radial(xy)))-1),a+t*max_a*((2*gray(angle(xy)))-1)])
end

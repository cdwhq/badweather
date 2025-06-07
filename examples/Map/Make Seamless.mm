# @title Make Seamless
# @tags map

# Edge Behaviour must be Wrap
filter org.mathmap.make_seamless (image in)
"Make sure you set `Edge Behaviour' to `Wrap'."
    ax=abs(x)/X;ay=abs(y)/Y;
    x1=max(0,ax-ay);
    y1=max(0,ay-ax);
    x2=min(1,ax+(1-ay));
    y2=min(1,ay+(1-ax));
    weight=clamp(1-(ax-x1)/(x2-x1),0,1);
    lerp(weight,in(xy+XY),in(xy))
end

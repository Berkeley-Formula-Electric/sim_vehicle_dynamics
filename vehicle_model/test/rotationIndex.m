function angle=rotationIndex(x, y, x0, y0)

    xVec = x-x0;
    yVec = y-y0;

    angle = atan(yVec/xVec);
    
    if xVec >= 0 && yVec >= 0
        angle = angle;
    elseif xVec >= 0 && yVec <= 0 
        angle = angle + 2*pi();
    elseif xVec <= 0 && yVec >= 0
        angle = angle + pi(); 
    else
        angle = angle + pi();

return;
end
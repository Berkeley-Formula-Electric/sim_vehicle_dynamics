function time=getTimeFromAng(target, v, wheelbase, turnAng)

time = 2*(target)*wheelbase/(v*sin(turnAng));

end

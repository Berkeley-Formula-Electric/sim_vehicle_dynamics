function time=getTimeFromAng(target, v, wheelbase, turnAng)

time = (target)*wheelbase/(v*sin(turnAng));

end

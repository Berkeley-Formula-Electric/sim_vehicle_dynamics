%% reset system, initialize track points, see file for layout
clear;
clc;
trackDataSimple;
nPoints = max(size(Length));
output = [0,0];
t = [5];
ang = [];
% velocity (m/s)
v = 10;

%% loop through each section of lap
for n=1:1:nPoints
   L = Length(n);
   R = Radius(n);
   DIR = Type(n);
   
   t = [t, t(end)+L/v];
   t0 = round(t(end-1),2);
   switch DIR
       case "Straight"
           ang = [ang,0];  
       case "Left"
           ang = [ang, L/R];
       case "Right"
           ang = [ang, -L/R];
            
   end
   ang0 = round(ang(end),2);
   output = [output; t0, ang0];
end

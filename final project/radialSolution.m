close all;
clear all; 
clc; 

g=0.3; 

Tmin=0;
Tmax=100;
Tsteps=20; 

T=linspace(Tmin,Tmax,Tsteps); 

W=30;
L=30; 

Spacing=100;
x=linspace(0,W,Spacing);
y=linspace(0,L,Spacing);

field=zeros(Spacing,Spacing);

for tstep=1:1:Tsteps
    for n=1:1:Spacing
        for m=1:1:Spacing
            
            r=((x(1,n)-W/2)^2+(y(1,m)-L/2)^2)^0.5;
            t=T(1,tstep); 
            
            %annoying issue with this solution is that invalid region
            %propagates out in r with t^2, so not really usable for
            %animation either 
            field(n,m)=t/r^2.5*(cos(g*t^2/(4*r))-sin(g*t^2/(4*r))); 
        end
    end
    
    figure;
    surf(x,y,field); 
    zlim([-10,10]); 
    pause(0.3);
end

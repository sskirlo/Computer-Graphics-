close all;
clear all;
clc; 

W=30;
L=30; 

Spacing=100;
x=linspace(0,W,Spacing);
y=linspace(0,L,Spacing); 


%number of modes we include
N=20;
M=20; 

%coeffecients for each eigenmode 
a=zeros(N,M);
omega=zeros(N,M); 

v=0.3; 

%location of delta function perturbation
x0=W/2; 
y0=L/2; 

for n=1:1:N
    for m=1:1:M
        a(n,m)=sin(pi*x0*n/W)*sin(pi*y0*m/L);  %calculate each coeffecient for delta initial condition 
        omega(n,m)=v*((pi*n/W)^2+(pi*m/L)^2)^0.5; 
    end
end

Tmin=0;
Tmax=3;
Tsteps=10; 

T=linspace(Tmin,Tmax,Tsteps);

field=zeros(Spacing,Spacing); 

%calculate evolution
for t=1:1:Tsteps
    
    %for each coordinate 
    for nx=1:1:Spacing
        for ny=1:1:Spacing
    
            %for each mode
            field(nx,ny)=0; 
            
            for n=1:1:N
                for m=1:1:M
                    field(nx,ny)=field(nx,ny)+a(n,m)*sin(pi*x(1,nx)*n/W)*sin(pi*y(1,ny)*m/L)*cos(omega(n,m)*T(1,t)); 
                end
            end
            
        end
    end

    figure;
    surf(x,y,field); 
    zlim([-20,20]); 
    
    pause(1); 
    
    
end
















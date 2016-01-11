close all;
clear all;
clc; 


%initialize height fields, start with 1D

N=100; 

h0=zeros(N,1);
h1=zeros(N,1); 


g=2;
dx=0.1;
dt=0.01; 

xmax=10; 
x=linspace(0,xmax,N);
dx=x(1,2)-x(1,1); 

b0=1.5; 

%initialize uniform depth 
b=ones(N,1)*b0; 

sig=xmax/5; 

for n=1:1:100
    
    %this creates gaussian wave initial condition 
    %h0(n,1)=2+0.2*exp(-(x(1,n)-x(1,N/4))^2/sig^2);
    %h1(n,1)=h0(n,1);
    
    h0(n,1)=2+0.1*exp(-(x(1,n)-x(1,N/2))^2/sig^2);
    h1(n,1)=h0(n,1);
    
    %h0(n,1)=2+0.2*sin(2*pi*omega*(x(1,n)-dt*(g/b0)^0.5));  %lets specify initial conditions for traveling wave  
    %h1(n,1)=2+0.2*sin(2*pi*omega*x(1,n)); 
    %h0(n,1)=h0(n,1)+0*sin(omega*n);  %initialize some random wave 
end

%{
for n=50:1:100
    h0(n,1)=2+0.0*sin(2*pi*omega*(x(1,n)-dt*(g/b0)^0.5));  %lets specify initial conditions for traveling wave  
    h1(n,1)=2+0.0*sin(2*pi*omega*x(1,n)); 
    %h0(n,1)=h0(n,1)+0*sin(omega*n);  %initialize some random wave 
end
%}

figure;
plot(x,h0);
title('h0');

figure;
plot(x,h1);
title('h1');

stepdiv=200; 
steps=1600; 

for n=1:1:steps
   
    %compute depth
    
    d=h1-b; 
    
    %compute A-matrix for implicit time-stepping
    
    A=sparse(N,N); 
    A(1,1)=1+g*dt^2*(d(1,1)+d(2,1) )/(2*dx^2); 
    A(N,N)=1+g*dt^2*(d(N,1)+d(N-1,1) )/(2*dx^2); 
    
    %fill in other elements along the diagonal
    for nx=2:1:(N-1) 
       A(nx,nx)= 1+g*dt^2*(d(nx-1,1)+2*d(nx,1)+d(nx+1,1) )/(2*dx^2); 
    end
   
    %fill in elements to right and left of diagonal
    for nx=1:1:(N-1)
        A(nx,nx+1)=-g*dt^2*(d(nx,1)+d(nx+1,1))/(2*dx^2); 
        A(nx+1,nx)=-g*dt^2*(d(nx,1)+d(nx+1,1))/(2*dx^2); 
    end
    
    %now we solve for next forward step 
    
    disp(h0);
    disp(h1); 
    
    hnew=A\(2*h1-h0); 
    
    h0=h1;
    h1=hnew; 
    
    if(mod(n-1,stepdiv)==0)
        figure;
        plot(x,h1); 
        %ylim([2-0.4,2+0.4]); 
        pause(0.05); 
    end
    
end









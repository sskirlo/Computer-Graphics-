close all;
clear all;
clc; 


%initialize height fields, start with 1D

N=100; 

h0=zeros(N,1);
h1=zeros(N,1); 

omega=1/5;
a=1; 

g=2;
xmax=10; 

dt=0.05; 

x=linspace(0,xmax,N);
dx=x(1,2)-x(1,1); 

b0=1.5; 

%initialize uniform depth 
%b=ones(N,1)*b0; 

%create sine pattern for depth 
b=b0/2*cos(4*pi/(xmax)*x)+b0/2;
b=b'; 

sig=xmax/20; 

%initialize fluid in only right "hump"
for n=50:1:N
    
    %this creates gaussian wave initial condition 
    h0(n,1)=b0*1.0+0.3*exp(-(x(1,n)-x(1,3*N/4))^2/sig^2);
    h1(n,1)=h0(n,1);
    
end



figure;
plot(x,h0);
title('h0');

figure;
plot(x,h1);
title('h1');

stepdiv=10; 
steps=400; 

for n=1:1:80
   
    %compute depth
    
    d=h1-b; 
    
    %check if any part of water is below height of land, i.e. 
    %d<0, and set d to 0 to correct, this does not conserve water volume
    for nx=1:1:N
        if(d(nx,1)<0)
            d(nx,1)=0; 
        end
    end
    
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
    
    %disp(h0);
    %disp(h1); 
    
    hnew=A\(2*h1-h0); 
    
    %calculate old and new volumes and offset
    %to correct for drift in volume 
    
    disp('step'); 
    disp(n);
    hnew=conserveVolume(h1,hnew,b); 
    
    h0=h1;
    h1=hnew; 
    
    epsilon=0.001; 
    %check condition 
    for nx=1:1:N
        if(h1(nx,1)<b(nx,1))
            h1(nx,1)=b(nx,1)-epsilon; 
            h0(nx,1)=b(nx,1)-epsilon; 
        end 
    end
    
    if(n==57 || n==68 || n==69)
    %if(mod(n-1,stepdiv)==0)
        figure;
        hold on; 
        area(x,h1,'FaceColor','b');  %plot water
        %area(x,b,'FaceColor','k'); %plot ground
        plot(x,b); 
        ylim([0,b0*1.5]); 
        pause(0.05); 
    end
    
end









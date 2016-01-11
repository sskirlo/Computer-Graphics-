close all;
clear all;
clc; 


%2D solver

N=100;
M=N; 

h0=zeros(N,M);
h1=zeros(N,M); 
hnew=zeros(N,M); 

g=2;
xmax=10; 
ymax=xmax; 

dt=0.2; 

x=linspace(0,xmax,N);
y=linspace(0,ymax,M);
dx=x(1,2)-x(1,1); 
dy=y(1,2)-y(1,1); 

b0=1.5; 

%initialize uniform depth 
b=ones(N,M)*b0; 

%create sine pattern for depth 
%b=b0/2*cos(4*pi/(xmax)*x)+b0/2;
%b=b'; 

sig=xmax/10; 

%initialize fluid in only right "hump"
for n=1:1:N
    for m=1:1:M
    
        %this creates gaussian wave initial condition 
        d2=(x(1,n)-x(1,N/4))^2+(y(1,m)-y(1,N/2))^2
        %need to make sure the waves are not too shallow!
        %this can cause weird numerical problems 
        h0(n,m)=b0+0.5+0.1*exp(-d2/sig^2);
        h1(n,m)=h0(n,m);
    
    end
end



figure;
surf(x,y,h0);
title('h0');

%figure;
%plot(x,h1);
%title('h1');

stepdiv=1; 
steps=20; 

%implement alternating direction method 

for n=1:1:steps
   
    %compute depth
    
    %do calculations along all rows
    for nx=1:1:M
        hnew(:,nx)=computeHnewX(b(:,nx),h0(:,nx),h1(:,nx),dx,N,g,dt);
        h0(:,nx)=h1(:,nx);
        h1(:,nx)=hnew(:,nx); 
    end
    
    
    %do calculations along all columns 
    for ny=1:1:M
        hnew(ny,:)=computeHnewX(b(ny,:)',h0(ny,:)',h1(ny,:)',dy,M,g,dt)';
        h0(ny,:)=h1(ny,:);
        h1(ny,:)=hnew(ny,:); 
    end
    
    
    %disp('step'); 
    %disp(n);
    %hnew=conserveVolume(h1,hnew,b); 
    
    %{
    %epsilon=0.001; 
    %%check condition 
    for nx=1:1:N
        if(h1(nx,1)<b(nx,1))
            h1(nx,1)=b(nx,1)-epsilon; 
            h0(nx,1)=b(nx,1)-epsilon; 
        end 
    end
    %}
    
    %if(n==57 || n==68 || n==69)
    if(mod(n-1,stepdiv)==0)
        figure;
        hold on; 
        surf(x,y,h1); 
        view(45,45); 
        grid off; 
        %area(x,h1,'FaceColor','b');  %plot water
        %area(x,b,'FaceColor','k'); %plot ground
        %plot(x,reshape(h1(:,50),[N,1])); 
        v=0.1; 
        zlim([b0+0.5-v,b0+0.5+v]); 
        pause(0.02); 
    end
    
end









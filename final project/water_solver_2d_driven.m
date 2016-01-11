close all;
clear all;
clc; 

%clear out png folder, 
png='pngfolder';
%rmdir(png); 
mkdir(png); 

mesh='meshfolder';
mkdir(mesh); 

%2D solver

N=100;
M=N; 

h0=zeros(N,M);
h1=zeros(N,M); 
hnew=zeros(N,M); 

g=2;
xmax=40; 
ymax=xmax; 

dt=0.01; %0.01

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
        d2=(x(1,n)-x(1,N/4))^2+(y(1,m)-y(1,N/2))^2; 
        %need to make sure the waves are not too shallow!
        %this can cause weird numerical problems 
        h0(n,m)=b0+0.5+0.0*exp(-d2/sig^2);
        h1(n,m)=h0(n,m);
    
    end
end



figure;
surf(x,y,h0);
title('h0');

%figure;
%plot(x,h1);
%title('h1');

stepdiv=20; 
steps=600*5;  %*5 

%specify several sin drivers in height field to make pretty drop effects,
%we can think about more elaborate things with this 

Ns=2;
pos=zeros(2,Ns);
amps=zeros(1,Ns);
freqs=zeros(1,Ns); 
offset=zeros(1,Ns); 

c=1; 
pos(:,c)=[20,80];
amps(1,c)=0.1;
freqs(1,c)=2*pi/(5); %driving frequency 
offset(1,c)=2.0; 
sigma(1,c)=2.5; %falloff for range of perturbation 
c=c+1;

pos(:,c)=[30,40];
amps(1,c)=0.1;
freqs(1,c)=2*pi/(5); %driving frequency 
offset(1,c)=2.0; 
sigma(1,c)=2.5; 
c=c+1; 

rag=3; 

%implement alternating direction method 

t=0; 
imagecount=0; 

for n=1:1:steps
   
    %compute depth
    
    t=t+dt; 
    
    
    %go through and insert height driving term 
    for ns=1:1:Ns
        %use gaussian falloff for range of perturbation to make look
        %prettier 
        %limit range we apply perturbation t0 +/- rag with gaussian weight
        for nx=(pos(1,ns)-rag):1:(pos(1,ns)+rag)
            for ny=(pos(2,ns)-rag):1:(pos(2,ns)+rag)
                d2=(x(1,nx)-x(1,pos(1,ns)))^2+(y(1,ny)-y(1,pos(2,ns)))^2; 
                amp=amps(1,ns)*exp(-d2/sigma(1,ns)^2);
                %disp(amp); 
                h1(pos(1,ns),pos(2,ns))=amp*sin(t*freqs(1,ns))+offset(1,ns); 
                %h1(pos(1,ns),pos(2,ns))=amp*sin(t*freqs(1,ns))+h1(pos(1,ns),pos(2,ns));  
            end
        end
        %disp(h1(pos(1,ns),pos(2,ns))); 
    end 
    
    
    
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
    
    hview=(h1-2)*100;  %put in reasonable scale for viewing as mesh, 
                      %we exaggerate features 
    
    %if(n==57 || n==68 || n==69)
    if(mod(n-1,stepdiv)==0)
        imagecount=imagecount+1; 
        f = figure('visible','off'); 
        hold on; 
        surf(x,y,hview); 
        view(45,45); 
        grid off; 
        %area(x,h1,'FaceColor','b');  %plot water
        %area(x,b,'FaceColor','k'); %plot ground
        %plot(x,reshape(h1(:,50),[N,1])); 
        
        v=0.2; 
        zlim([-2,2]); 
        pause(0.02); 
        
        createObj([mesh,'/water_mesh',sprintf('%d',imagecount),'.obj'],x,y,hview)
        %print([png,'/image',sprintf('%d',imagecount)],'-dpng'); 
        fprintf('%d of %d complete \n',imagecount,floor(steps/stepdiv));
    end
    
end







